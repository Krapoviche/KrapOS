#ifndef __PRIMITIVE_H__
#define __PRIMITIVE_H__
#include "stdint.h"
#define MAX_STACK_SIZE 8192

extern void ctx_sw(uint32_t* old, uint32_t* new);

int start(int (*ptfunc)(void *), unsigned long ssize, int prio, const char *name, void *arg);
int getprio(int pid);
int chprio(int pid, int newprio);
int getpid(void);
int getppid(void);
void wait_clock(uint32_t clock);
int waitpid(int pid, int *retvalp);
void exit(int retval);
int kill(int pid);
int pcreate(int count);
int pcount(int fid, int* count);
int psend(int fid, int message);
int preceive(int fid, int* message);
int preset(int fid);
int pdelete(int fid);
#endif