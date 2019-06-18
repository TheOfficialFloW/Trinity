/* rop.c -- rop chain utility
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
#include "rop.h"

#define MAX_BASES 16

static int rop_bases[MAX_BASES];
static void *rop_buf;
static int rop_size;
static int rop_seek;
static int rop_begin = 0;

int register_base(int index, int base) {
  if (index >= MAX_BASES)
    return -1;

  rop_bases[index] = base;
  return 0;
}

int begin_rop(void *buf, int size) {
  if (rop_begin)
    return -1;

  register_base(0, 0);
  rop_buf = buf;
  rop_size = size;
  rop_seek = 0;
  rop_begin = 1;

  return 0;
}

int end_rop(void) {
  if (!rop_begin)
    return -1;

  int size = rop_seek;

  rop_buf = NULL;
  rop_size = 0;
  rop_seek = 0;
  rop_begin = 0;

  return size;
}

int push(int base_index, int gadget) {
  if (!rop_begin || rop_seek >= rop_size || base_index >= MAX_BASES)
    return -1;

  *(int *)(rop_buf + rop_seek) = rop_bases[base_index] + gadget;
  rop_seek += 4;

  return 0;
}
