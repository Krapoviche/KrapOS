#define SYS_MILLISLEEP 35
#define SYS_GETPID 39
#define SYS_START 56 // sys_clone in linux
#define SYS_EXIT 60
#define SYS_KILL 62
#define SYS_GETPPID 110
#define SYS_GETPRIORITY 140
#define SYS_SETPRIORITY 141
#define SYS_CLOCK_GETTIME 228
#define SYS_CLOCK_GETRES 229 // sys_clock_getres in linux
#define SYS_MQ_OPEN 240
#define SYS_MQ_UNLINK 241
#define SYS_MQ_SEND 242
#define SYS_MQ_RECEIVE 243
#define SYS_MQ_RESET 244 // sys_notify in linux
#define SYS_MQ_COUNT 245 // sys_mq_getsetattr in linux
#define SYS_WAITID 247
// Custom syscalls not inspired by linux syscalls
#define SYS_WAITCLOCK 300
#define SYS_CONSREAD 301
#define SYS_CONSWRITE 302
#define SYS_CONSECHO 303
#define SYS_PS 304
#define SYS_DUMP_STACK 305


#define SYSCALL0(name, SYSCALLID) \
    .globl name; \
name: \
    pushl   %eax; \
    movl	$SYSCALLID, %eax; \
    int    $49; \
    addl    $4, %esp; \
    ret

#define SYSCALL1(name, SYSCALLID) \
    .globl name; \
name: \
    pushl   %eax; \
    pushl   %ebx; \
    movl	$SYSCALLID, %eax; \
    movl	12(%esp), %ebx; \
    int    $49; \
    popl    %ebx; \
    addl    $4, %esp; \
    ret

#define SYSCALL2(name, SYSCALLID) \
    .globl name; \
name: \
    pushl   %eax; \
    pushl   %ebx; \
    pushl   %ecx; \
    movl	$SYSCALLID, %eax; \
    movl	16(%esp), %ebx; \
    movl	20(%esp), %ecx; \
    int    $49; \
    popl    %ecx; \
    popl    %ebx; \
    addl    $4, %esp; \
    ret

#define SYSCALL3(name, SYSCALLID) \
    .globl name; \
name: \
    pushl   %eax; \
    pushl   %ebx; \
    pushl   %ecx; \
    pushl   %edx; \
    movl	$SYSCALLID, %eax; \
    movl	20(%esp), %ebx; \
    movl	24(%esp), %ecx; \
    movl	28(%esp), %edx; \
    int    $49; \
    popl    %edx; \
    popl    %ecx; \
    popl    %ebx; \
    addl    $4, %esp; \
    ret

#define SYSCALL4(name, SYSCALLID) \
    .globl name; \
name: \
    pushl   %eax; \
    pushl   %ebx; \
    pushl   %ecx; \
    pushl   %edx; \
    pushl   %edi; \
    movl	$SYSCALLID, %eax; \
    movl	24(%esp), %ebx; \
    movl	28(%esp), %ecx; \
    movl	32(%esp), %edx; \
    movl	36(%esp), %edi; \
    int    $49; \
    popl    %edi; \
    popl    %edx; \
    popl    %ecx; \
    popl    %ebx; \
    addl    $4, %esp; \
    ret

#define SYSCALL5(name, SYSCALLID) \
    .globl name; \
name: \
    pushl   %eax; \
    pushl   %ebx; \
    pushl   %ecx; \
    pushl   %edx; \
    pushl   %edi; \
    pushl   %esi; \
    movl	$SYSCALLID, %eax; \
    movl	28(%esp), %ebx; \
    movl	32(%esp), %ecx; \
    movl	36(%esp), %edx; \
    movl	40(%esp), %edi; \
    movl	44(%esp), %esi; \
    int    $49; \
    popl    %esi; \
    popl    %edi; \
    popl    %edx; \
    popl    %ecx; \
    popl    %ebx; \
    addl    $4, %esp; \
    ret


SYSCALL0(getpid, SYS_GETPID)
SYSCALL0(getppid, SYS_GETPPID)
SYSCALL0(current_clock, SYS_CLOCK_GETTIME)
SYSCALL0(ps, SYS_PS)
SYSCALL0(dump_stack, SYS_DUMP_STACK)

SYSCALL1(millisleep, SYS_MILLISLEEP)
SYSCALL1(getprio, SYS_GETPRIORITY)
SYSCALL1(exit, SYS_EXIT)
SYSCALL1(kill, SYS_KILL)
SYSCALL1(pcreate, SYS_MQ_OPEN)
SYSCALL1(pdelete, SYS_MQ_UNLINK)
SYSCALL1(preset, SYS_MQ_RESET)
SYSCALL1(wait_clock, SYS_WAITCLOCK)
SYSCALL1(cons_echo, SYS_CONSECHO)

SYSCALL2(chprio, SYS_SETPRIORITY)
SYSCALL2(psend, SYS_MQ_SEND)
SYSCALL2(preceive, SYS_MQ_RECEIVE)
SYSCALL2(pcount, SYS_MQ_COUNT)
SYSCALL2(waitpid, SYS_WAITID)
SYSCALL2(clock_settings, SYS_CLOCK_GETRES)
SYSCALL2(cons_read, SYS_CONSREAD)
SYSCALL2(cons_write, SYS_CONSWRITE)

SYSCALL5(start, SYS_START)