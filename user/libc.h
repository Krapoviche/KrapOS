#ifndef __LIBC_H__
#define __LIBC_H__
#include "stdint.h"

int start(int (*ptfunc)(void *), unsigned long ssize, int prio, const char *name, void *arg);
int getpid(void);
int getppid(void);
int getprio(int pid);
int chprio(int pid, int prio);
void millisleep(unsigned long msecs);
void sleep(unsigned long secs);
int waitpid(int pid, int* retvalp);
int wait(int* retvalp);
void exit(int retval);
int kill(int pid);
int pcreate(int size);
int pdelete(int fid);
int psend(int fid, int message);
int preceive(int fid, int* message);
int preset(int fid);
int pcount(int fid, int* count);

void console_putbytes(const char *s, int len);

#endif
