#ifndef __MAIN_H__
#define __MAIN_H__

#define printf pspDebugScreenPrintf

#define ALIGN(x, align) (((x) + ((align) - 1)) & ~((align) - 1))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#endif
