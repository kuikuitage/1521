#ifndef _INTF_H_
#define _INTF_H_

extern char* mt_strncpy(char *dst, const char *src, unsigned int n);
extern void* mt_memset (void *str, int c, int len);
extern void wr_reg8(unsigned int unAddr, unsigned char ucDataw);
extern unsigned char rd_reg8(unsigned int unAddr);

typedef unsigned int size_t;

#endif
