#include "debugger.h"
#include "cpu.h"
#include "screen.h"
#include "it.h"
#include "process.h"
#include "stdio.h"
#include "string.h"
#include "mem.h"

void proc1(void) {
  for (;;) {
    printf("[%s] pid = %i\n", get_name(), get_pid());
    for (int32_t i = 0; i < 100000000; i++)
      ;
    scheduler();
  }
}

void kernel_start(void)
{
	// call_debugger(); useless with qemu -s -S
	uint32_t registers [5];

	reset_screen();

	process_table = init_process_table();
	process_t * p;

	for(int i = 0 ; i < 30 ; i++){
		p = mem_alloc(sizeof(process_t));
		sprintf(p->name, "p_%d", i);
		p->priority = i;
		add_process(p, (uint32_t)&proc1);
	}

	process_table->running = queue_out(process_table->runnable_queue, process_t, queue_link);
	process_table->running->state = RUNNING;

	ctx_sw(registers, process_table->running->register_save_zone);

	return;
}
