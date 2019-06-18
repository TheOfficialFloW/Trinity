/* main.c -- Trinity main
 *
 * Copyright (C) 2019 TheFloW
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <pspsdk.h>
#include <pspkernel.h>
#include <pspctrl.h>
#include <pspwlan.h>

#include <stdio.h>
#include <string.h>

#include "main.h"
#include "rain.h"
#include "mips.h"
#include "arm.h"

PSP_MODULE_INFO("Trinity", 0, 1, 0);

__attribute__((__used__)) char *message = "Hi Sony! Hire me now?";

int main(int argc, char *argv[]) {
  int res;
  SceCtrlData pad;

  pspDebugScreenInit();

  sceCtrlPeekBufferPositive(&pad, 1);
  if (pad.Buttons & PSP_CTRL_LTRIGGER) {
    matrix_rain(NULL);
    return 0;
  }

  if (!sceWlanGetSwitchState()) {
    pspDebugScreenSetXY(17, 15);
    printf("[TURN ON WI-FI TO USE THIS EXPLOIT]");
    sceKernelDelayThread(10 * 1000 * 1000);
    sceKernelExitGame();
    return 0;
  }

  res = exploit_mips_kernel();
  if (res < 0)
    goto err;

  res = rain_animation();
  if (res < 0)
    goto err;

  res = exploit_arm_userland();
  if (res < 0)
    goto err;

  res = exploit_arm_kernel();
  if (res < 0)
    goto err;

err:
  if (res < 0) {
    pspDebugScreenClear();
    pspDebugScreenSetXY(20, 15);
    printf("[EXPLOIT FAILED: 0x%08X]", res);
  }

  sceKernelDelayThread(10 * 1000 * 1000);
  sceKernelExitGame();
  return 0;
}
