#include "debugger.h"
#include "cpu.h"
#include "screen.h"
#include "it.h"
#include "process.h"
#include "stdio.h"
#include "string.h"
#include "mem.h"

void idle() {
  for (;;) {
    sti();
    hlt();
    cli();
  }
}

void proc1(void) {
  for (;;) {
    printf("[%s] pid = %i\n", get_name(), get_pid());
    sleep(2);
  }
}

void proc2(void) {
  for (;;) {
    printf("[%s] pid = %i\n", get_name(), get_pid());
    sleep(4);
  }
}

void proc3(void) {
  for (;;) {
    printf("[%s] pid = %i\n", get_name(), get_pid());
    sleep(7);
  }
}

void proc4(void) {
  for (;;) {
    printf("[%s] pid = %i\n", get_name(), get_pid());
    sleep(10);
  }
}

void kernel_start(void)
{
	// call_debugger(); useless with qemu -s -S
	uint32_t registers [5];

	reset_screen();
	place_cursor(0, 0);

	init_clock();
	init_IT_handlers(32, IT_32_handler);
	mask_IRQ(0, false);

	process_table = init_process_table();
	process_t * p;

	p = mem_alloc(sizeof(process_t));
	sprintf(p->name, "p_%d", 0);
	p->priority = 1;
	add_process(p, (uint32_t)&idle);

	p = mem_alloc(sizeof(process_t));
	sprintf(p->name, "p_%d", 1);
	p->priority = 1;
	add_process(p, (uint32_t)&proc1);

	p = mem_alloc(sizeof(process_t));
	sprintf(p->name, "p_%d", 2);
	p->priority = 1;
	add_process(p, (uint32_t)&proc2);

	p = mem_alloc(sizeof(process_t));
	sprintf(p->name, "p_%d", 3);
	p->priority = 1;
	add_process(p, (uint32_t)&proc3);

	p = mem_alloc(sizeof(process_t));
	sprintf(p->name, "p_%d", 4);
	p->priority = 1;
	add_process(p, (uint32_t)&proc4);	

	process_table->running = queue_out(process_table->runnable_queue, process_t, queue_link);
	process_table->running->state = RUNNING;

	ctx_sw(registers, process_table->running->register_save_zone);

	return;
}
