#include "syscall.h"

void user_start(void)
{
	// call_debugger(); useless with qemu -s -S

    getpid();
    getprio();

    // printf("%d",pid);

    // syscall_handler(pid);

    while(1){};

	return;
}
