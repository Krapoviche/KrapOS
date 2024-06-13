#ifndef __SYSCALL_H__
#define __SYSCALL_H__

int getpid(void);
int getprio(int pid);
void console_putbytes(const char *s, int len);

#endif
