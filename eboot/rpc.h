#ifndef __RPC_H__
#define __RPC_H__

#define NATIVE(addr) (((u32)(addr) & 0x1fffffff) - 0x08000000 + 0x70000000)

#define ME_CMD_CSC_ROW  0x151
#define ME_CMD_CSC_INIT 0x152

#define KERMIT_MODE_WLAN 0xa

#define KERMIT_CMD_ADHOC_CREATE 0x4e

#define KERMIT_INPUT_MODE  1
#define KERMIT_OUTPUT_MODE 2

typedef struct {
  u32 cmd;
  SceUID semaid;
  u64 *resp;
  u32 pad;
  u64 args[14];
} KermitRequest;

typedef struct {
  u8 mac[6];
  u8 channel;
  u8 bufsize;
  u8 buf[0x100];
} KermitAdhocCreateParam;

extern int (* read_native)(void *dst, u32 src, size_t len);
extern int (* syscall)(int num, ...);
extern int (* call)(int func, ...);
extern int (* init_rpc)(u32 cookie, u32 pspemu_base, u32 libkernel_base);

int _read_native(void *dst, u32 src, size_t len);
int _syscall(int num, ...);
int _call(int func, ...);
int _init_rpc(u32 cookie, u32 pspemu_base, u32 libkernel_base);

void push_call(int func, ...);
void push_syscall(int num, ...);

#endif
