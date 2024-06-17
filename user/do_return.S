    .section .text.do_return
    .global do_return
do_return:
    pushl	%eax
    call   exit
    ret