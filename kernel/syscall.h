#ifndef __SYSCALL_H__
#define __SYSCALL_H__

void* test_it49(int sn, int arg1, int arg2, int arg3, int arg4, int arg5);

#include "it.h"
#include "primitive.h"
#include "process.h"
#include "semaphore.h"
#include "kbd.h"
#include "debugger.h"
#include "ps.h"
#include "pci.h"

typedef enum syscall{
    SYS_MILLISLEEP = 35,
    SYS_GETPID = 39,
    SYS_START = 56,
    SYS_EXIT = 60,
    SYS_KILL = 62,
    SYS_SEMCREATE = 63,
    SYS_SEMDELETE = 64,
    SYS_SEMSIGNAL = 65,
    SYS_SEMSWAIT = 66,
    SYS_SEMRESET = 67,
    SYS_SEMCOUNT = 68,
    SYS_SEMTRYWAIT = 69,
    SYS_GETPPID = 110,
    SYS_GETPRIORITY = 140,
    SYS_SETPRIORITY = 141,
    SYS_CLOCK_GETTIME = 228,
    SYS_CLOCK_GETRES = 229,
    SYS_MQ_OPEN = 240,
    SYS_MQ_UNLINK = 241,
    SYS_MQ_SEND = 242,
    SYS_MQ_RECEIVE = 243,
    SYS_MQ_RESET = 244,
    SYS_MQ_COUNT = 245,
    SYS_WAITID = 247,
    SYS_WAITCLOCK = 300,
    SYS_CONSREAD = 301,
    SYS_CONSWRITE = 302,
    SYS_CONSECHO = 303,
    SYS_PS = 304,
    SYS_DUMP_STACK = 305
} syscall;

#endif
