void* test_it49(int sn, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6);

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
    SYS_CLOCK_GETTIME = 228,
    SYS_CLOCK_GETRES = 229,
    SYS_MQ_OPEN = 240,
    SYS_MQ_UNLINK = 241,
    SYS_MQ_SEND = 242,
    SYS_MQ_RECEIVE = 243,
    SYS_MQ_RESET = 244,
    SYS_MQ_COUNT = 245,
    SYS_WAITID = 247,
    SYS_WAITCLOCK = 300
} syscall;