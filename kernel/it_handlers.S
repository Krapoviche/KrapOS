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