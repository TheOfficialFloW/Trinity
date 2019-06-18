/* gadgets.h -- rop gadgets
 *
 * Copyright (C) 2019 TheFloW
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef __GADGETS_H__
#define __GADGETS_H__

// ScePspemu gadgets
#define ScePspemu_str_r0_r1_bx_lr                   0x1e78f
#define ScePspemu_str_r2_r1_bx_lr                   0x1e7b5
#define ScePspemu_str_r3_r0_movs_r0_0_bx_lr         0x1a8a7

#define ScePspemu_mov_lr_r3_bx_lr                   0x9

#define ScePspemu_syscall                           0x6af0
#define ScePspemu_remoteNetAdhocCreate_lr           0x12f35

// SceLibKernel gadgets
#define SceLibKernel_pop_pc                         0x9e7
#define SceLibKernel_pop_r0_pc                      0x141
#define SceLibKernel_pop_r0_r1_pc                   0xd03
#define SceLibKernel_pop_r1_r2_r3_pc                0x3b5
#define SceLibKernel_pop_r3_pc                      0x14b1
#define SceLibKernel_pop_r4_r5_r6_r7_pc             0xad7d
#define SceLibKernel_pop_ip_pc                      0x558 // arm

#define SceLibKernel_mov_ip_r0_bx_lr                0x809
#define SceLibKernel_mov_sp_ip_pop_pc               0x1111

#define SceLibKernel_add_sp_8_pop_r4_pc             0x13a3
#define SceLibKernel_add_r1_sp_5c_blx_r7            0x7bbf
#define SceLibKernel_subs_r0_r1_r0_pop_pc           0x9e5

// SceSysmem gadgets
#define SceSysmem_blx_r0                            0x1fb3f
#define SceSysmem_blx_r4_pop_r4_pc                  0x19fb9

#define SceSysmem_pop_pc                            0x347
#define SceSysmem_pop_r0_r1_pc                      0x853
#define SceSysmem_pop_r0_r1_r2_r3_r4_pc             0x258bd
#define SceSysmem_pop_r3_pc                         0x1d8f
#define SceSysmem_pop_r3_r4_r5_pc                   0x1377
#define SceSysmem_ldr_r0_r0_blx_r3                  0x11337
#define SceSysmem_ldr_r2_r5_14_blx_r3               0x1f697
#define SceSysmem_ldm_r0_r4_sl_ip_sp_pc             0x1e460 // arm

#define SceSysmem_str_r0_r4_movs_r0_0_pop_r4_pc     0x4f71

#define SceSysmem_adds_r0_1_bx_lr                   0x533b

#define SceSysmem_ksceKernelGetMemBlockBase         0x5849
#define SceSysmem_ksceKernelAllocMemBlock           0x7d3d
#define SceSysmem_ksceKernelRemapBlock              0x7f69
#define SceSysmem_ksceKernelFreeMemBlock            0x825d
#define SceSysmem_ksceDeflateDecompress             0x1c3bb
#define SceSysmem_ksceKernelMemcpyUserToKernel      0x6289
#define SceSysmem_ksceKernelCpuDcacheWritebackRange 0x22fcd

#define SceSysmem_empty_string                      0x19

#endif
