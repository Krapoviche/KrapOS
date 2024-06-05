#include "debugger.h"
#include "cpu.h"
#include "screen.h"
#include "it.h"
#include "process.h"
#include "stdio.h"
#include "string.h"
#include "mem.h"

void proc1(void) {
	for (int i = 0; i < 3 ; i++) {
		printf("[%s] pid = %i, i = %d\n", get_name(), get_pid(), i);
		sleep(i + 1);
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

	process_t * p_idle = mem_alloc(sizeof(process_t));
	sprintf(p_idle->name, "idle");
	p_idle->priority = INT32_MIN;
	add_process(p_idle, (uint32_t)&idle);

	process_table->running = queue_out(process_table->runnable_queue, process_t, queue_link);
	process_table->running->state = RUNNING;

	process_t * p;
	for(int i=0 ; i < 150 ; i++){
		p = mem_alloc(sizeof(process_t));
		sprintf(p->name, "p_%d", i);
		p->priority = 1;
		add_process(p, (uint32_t)&proc1);
	}

	idle();
	return;
}
