#ifndef __ARM_H__
#define __ARM_H__

typedef struct SceKernelSegmentInfo {
  SceUInt size;   //!< sizeof(SceKernelSegmentInfo)
  SceUInt perms;  //!< probably rwx in low bits
  void *vaddr;    //!< address in memory
  SceUInt memsz;  //!< size in memory
  SceUInt flags;  //!< meaning unknown
  SceUInt res;    //!< unused?
} SceKernelSegmentInfo;

typedef struct SceKernelModuleInfoNative {
  SceUInt size;                       //!< 0x1B8 for Vita 1.x
  SceUInt handle;                     //!< kernel module handle?
  SceUInt flags;                      //!< some bits. could be priority or whatnot
  char module_name[28];
  SceUInt unk28;
  void *module_start;
  SceUInt unk30;
  void *module_stop;
  void *exidxTop;
  void *exidxBtm;
  SceUInt unk40;
  SceUInt unk44;
  void *tlsInit;
  SceSize tlsInitSize;
  SceSize tlsAreaSize;
  char path[256];
  SceKernelSegmentInfo segments[4];
  SceUInt type;                       //!< 6 = user-mode PRX?
} SceKernelModuleInfoNative;

typedef struct SceDisplayFrameBuf {
  SceSize size;               //!< sizeof(SceDisplayFrameBuf)
  void *base;                 //!< Pointer to framebuffer
  unsigned int pitch;         //!< pitch pixels
  unsigned int pixelformat;   //!< pixel format (one of ::SceDisplayPixelFormat)
  unsigned int width;         //!< framebuffer width
  unsigned int height;        //!< framebuffer height
  unsigned int unk;           //!< unknown
} SceDisplayFrameBuf;

int exploit_arm_kernel(void);
int exploit_arm_userland(void);

#endif
