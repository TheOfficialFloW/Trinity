/* rain.c -- matrix rain effect
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
#include <stdlib.h>
#include <time.h>

#include "main.h"

#define WIDTH  60
#define HEIGHT 34

static int randint(int low, int high) {
  return rand() % (high - low + 1) + low;
}

void matrix_rain(const char *string) {
  char m[WIDTH][HEIGHT];
  char p[WIDTH];
  int x, y;
  int count;

  int str_len = string ? strlen(string) : 0;
  int str_x = (WIDTH - str_len) / 2;
  int str_y = 15;
  int state = 0;

  memset(m, 0, sizeof(m));
  for (x = 0; x < WIDTH; x++) {
    p[x] = randint(0, HEIGHT);
  }

  srand(time(NULL));
  pspDebugScreenClear();

  while (1) {
    // Draw
    for (y = 0; y < HEIGHT; y++) {
      for (x = 0; x < WIDTH; x++) {
        char tail = p[x];
        char head = p[x] + HEIGHT - 1;

        if (y >= tail && y <= head) {
          u8 g = (u8)(((float)(y - tail) * 255.0f) / (float)HEIGHT);
          u32 color = (y == head) ? 0xFFFFFF : (g << 8);

          if (string) {
            if (m[x][str_y] != '\0' && m[x][str_y] != ' ') {
              if (x >= str_x && x < (str_x + str_len)) {
                if (y == str_y) {
                  color = 0xFFFFFF;
                } else if (y > str_y) {
                  color = 0;
                }
              }
            }
          }

          pspDebugScreenPutChar(x * 8, y * 8, color, m[x][y]);
        }
      }
    }

    // Update
    count = 0;
    for (x = 0; x < WIDTH; x++) {
      if (p[x] < HEIGHT) {
        p[x]++;
      } else {
        if (state == 0) {
          // Letter already generated
          if (string) {
            if (x >= str_x && x < (str_x + str_len)) {
              if (m[x][str_y]) {
                count++;
                if (count == str_len)
                  state = 1;
                continue;
              }
            }
          }

          // New trail
          if (randint(0, 20) == 0) {
            p[x] = -HEIGHT;
            for (y = 0; y < HEIGHT; y++) {
              m[x][y] = randint(0x20, 0x80);

              // Generate letter
              if (string) {
                if (x >= str_x && x < (str_x + str_len)) {
                  if (y == str_y) {
                    m[x][y] = string[x - str_x];
                  }
                }
              }
            }
          }
        } else if (state == 1) {
          count++;
        }
      }
    }

    // All letters generated and all trails fallen
    if (count == WIDTH)
      break;

    sceKernelDelayThread(30 * 1000);
  }
}

extern int trigger_exploit;

static int rain_thread(SceSize args, void *argp) {
  matrix_rain("Trinity Exploit by TheFloW");
  trigger_exploit = 1;
  return sceKernelExitDeleteThread(0);
}

int rain_animation(void) {
  SceUID thid = sceKernelCreateThread("", rain_thread, 0x20, 0x1000, 0, NULL);
  if (thid < 0)
    return thid;

  sceKernelStartThread(thid, 0, NULL);
  return 0;
}
