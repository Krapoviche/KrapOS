#include "debugger.h"
#include "cpu.h"
#include "screen.h"
#include "it.h"
#include "process.h"
#include "message.h"
#include "stdio.h"
#include "string.h"
#include "mem.h"
#include "tests/TEST_CASE"


void idle(void){
	while(1){
		sti();
		hlt();
		cli();
		exit(0);
	}
}

void kernel_start(void)
{
	do_nothing();
	// call_debugger(); useless with qemu -s -S

	reset_screen();
	place_cursor(0, 0);

	init_clock();
	init_IT_handlers(32, IT_32_handler);
	mask_IRQ(0, false);
	process_table = init_process_table();

	start((void*)idle, 256, INT32_MIN, "p_idle", 0);

	process_table->running = queue_out(process_table->runnable_queue, process_t, queue_link);
	process_table->running->state = RUNNING;

	start((void*)test, 256, 128, "test", 0);

	idle();
	return;
}
