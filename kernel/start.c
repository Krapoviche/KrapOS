#include "debugger.h"
#include "cpu.h"
#include "screen.h"
#include "it.h"
#include "process.h"
#include "stdio.h"
#include "string.h"
#include "mem.h"

int fact(int n)
{
	if (n < 2)
		return 1;

	return n * fact(n-1);
}

void idle(void) {
  for (;;) {
    printf("[%s] pid = %i\n", get_name(), get_pid());
    for (int32_t i = 0; i < 100000000; i++)
      ;
    scheduler();
  }
}

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

	process_t * p_idle = mem_alloc(sizeof(process_t));
	sprintf(p_idle->name, "p_idle");
	add_process(p_idle, (uint32_t)&idle);

	process_t* p_proc1 = mem_alloc(sizeof(process_t)); 
	sprintf(p_proc1->name, "p_proc1");
	add_process(p_proc1, (uint32_t)&proc1);

	process_table->running = &process_table->procs[0];
	process_table->procs[0].state = RUNNING;

	ctx_sw(registers, process_table->procs[0].register_save_zone);

	return;
}
