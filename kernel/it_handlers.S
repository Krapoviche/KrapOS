    .globl do_iret
do_iret:
    iret

    .globl IT_32_handler
IT_32_handler:
    pushl %eax
    pushl %edx
    pushl %ecx

    call tic_PIT

    popl %ecx
    popl %edx
    popl %eax

    iret

    .globl IT_49_handler
IT_49_handler:
    pushl %esi
    pushl %edi
    pushl %edx
    pushl %ecx
    pushl %ebx
    pushl %eax

    call test_it49

    addl $4, %esp
    popl %ebx
    popl %ecx
    popl %edx
    popl %edi
    popl %esi

    iret
