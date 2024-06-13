#ifndef __START_H__
#define __START_H__

#define FIRST_STACK_SIZE 16384

#ifndef ASSEMBLER

extern char first_stack[FIRST_STACK_SIZE];

/* This is the first user process to start. It is mapped to the entry point
of the user code. */
int user_start(void *);

/* The kernel entry point */
void kernel_start(void);

typedef enum syscall{
    SYS_WRITE = 1,
    SYS_MILLISLEEP = 35,
    SYS_GETPID = 39,
    SYS_START = 56,
    SYS_EXIT = 60,
    SYS_KILL = 62,
    SYS_GETPPID = 110,
    SYS_GETPRIORITY = 140,
    SYS_SETPRIORITY = 141,
    SYS_MQ_OPEN = 240,
    SYS_MQ_UNLINK = 241,
    SYS_MQ_SEND = 242,
    SYS_MQ_RECEIVE = 243,
    SYS_MQ_RESET = 244,
    SYS_MQ_COUNT = 245,
    SYS_WAITID = 247
} syscall;

#endif

#endif