#ifndef __ROP_H__
#define __ROP_H__

int register_base(int index, int base);
int begin_rop(void *buf, int size);
int end_rop(void);
int push(int base_index, int gadget);

#endif
