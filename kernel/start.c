#include "debugger.h"
#include "cpu.h"
#include "screen.h"
#include <stdio.h>

int fact(int n)
{
	if (n < 2)
		return 1;

	return n * fact(n-1);
}


void kernel_start(void)
{
	// call_debugger(); useless with qemu -s -S

    reset_screen();
    place_cursor(0,0);
	printf("Hello World from vandepuj & telliere kernel !");

	while(1)
	  hlt();

	return;
}
