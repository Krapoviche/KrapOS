#include "debugger.h"
#include "cpu.h"
#include "screen.h"
#include "it.h"
#include "process.h"
#include "stdio.h"
#include "string.h"
#include "mem.h"

void proc1(uint32_t j, uint32_t k) {
	printf("param1 %d\n", j);
	printf("param2 %d\n", k);
	for (int i = 0; i < 3 ; i++) {
		printf("[%s] pid = %i, i = %d\n", get_name(), get_pid(), i);
		sleep(j + 1);
	}
	printf("[%s] pid = %i, END\n", get_name(), get_pid());
}

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
	mask_IRQ(0, false);
	process_table = init_process_table();

	start((void*)idle, 256, INT32_MIN, "p_idle", 0);

	process_table->running = queue_out(process_table->runnable_queue, process_t, queue_link);
	process_table->running->state = RUNNING;

	for(int i=0 ; i < 15 ; i++){
		char name[MAX_PROC_NAME_SIZE];
		sprintf(name, "proc_%d", i);
		start((void*)proc1, 256, 1, name, 2, i, i+1);
	}

	idle();
	return;
}
