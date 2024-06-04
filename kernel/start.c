#include "debugger.h"
#include "cpu.h"
#include "screen.h"
#include "it.h"
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
	init_clock();
	init_IT_handlers(32, IT_32_handler);
	mask_IRQ(0, false);

	sti();

	while(1)
	  hlt();

	return;
}
