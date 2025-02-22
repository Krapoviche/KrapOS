#include "debugger.h"
#include "cpu.h"
#include "screen.h"
#include "it.h"
#include "process.h"
#include "stdio.h"
#include "primitive.h"

void idle(void){
	while(1){
		sti();
		hlt();
		cli();
	}
}

void kernel_start(void)
{
	// call_debugger(); useless with qemu -s -S

	reset_screen();
	place_cursor(0, 0);

	init_clock();
	init_IT_handlers(32, IT_32_handler);
	init_IT_handlers(49, IT_49_handler);
	mask_IRQ(0, false);
	process_table = init_process_table();

	start((void*)idle, 256, INT32_MIN, "p_idle", 0);

	process_table->running = queue_out(process_table->runnable_queue, process_t, queue_link);
	process_table->running->state = RUNNING;

	void* user_start = (void*)0x1000000;
	start(user_start, 2048, 1, "user_start", 0);


	idle();
	return;
}
