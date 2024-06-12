#define GETPID 0
#define GETPRIO 2


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
    movl	16(%esp), %ebx; \
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
    movl	20(%esp), %ebx; \
    movl	24(%esp), %ecx; \
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
    movl	24(%esp), %ebx; \
    movl	28(%esp), %ecx; \
    movl	32(%esp), %edx; \
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
    movl	28(%esp), %ebx; \
    movl	32(%esp), %ecx; \
    movl	36(%esp), %edx; \
    movl	40(%esp), %edi; \
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
    movl	32(%esp), %ebx; \
    movl	36(%esp), %ecx; \
    movl	40(%esp), %edx; \
    movl	44(%esp), %edi; \
    movl	48(%esp), %esi; \
    int    $49; \
    popl    %esi; \
    popl    %edi; \
    popl    %edx; \
    popl    %ecx; \
    popl    %ebx; \
    addl    $4, %esp; \
    ret


SYSCALL0(getpid, GETPID)
SYSCALL0(getprio, GETPRIO)
