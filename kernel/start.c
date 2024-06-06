#include "debugger.h"
#include "cpu.h"
#include "screen.h"
#include "it.h"
#include "process.h"
#include "stdio.h"
#include "string.h"
#include "mem.h"


void proc12() {
	sleep(2);
	printf("exit child\n");
	exit(4);
}

void proc13() {
	sleep(2);
	printf("exit child\n");
	exit(99);
}

void proc1() {
	cli();
	sti();
	int retval;
	char name[MAX_PROC_NAME_SIZE];
	int32_t cpid;
	for (int i = 0; i < 3 ; i++) {
		sprintf(name, "proc_%d", i+10);
		cpid = start((void*)proc12, 256, 3, name, 0);
		cli();
		printf("created process pid=%d, parent_pid=%d\n", cpid, process_table->table[cpid]->ppid);
		sti();
	}
	cpid = start((void*)proc13, 256, 3, "proc_999", 0);
	cli();
	printf("created process pid=%d, parent_pid=%d\n", cpid, process_table->table[cpid]->ppid);
	sti();
	waitpid(cpid, &retval);
	printf("child (pid %d) returned : %d\n", cpid, retval);
}

void proc2() {
	cli();
	sti();
	int retval;
	char name[MAX_PROC_NAME_SIZE];
	int32_t cpid;
	for (int i = 0; i < 3 ; i++) {
		sprintf(name, "proc_%d", i+10);
		cpid = start((void*)proc12, 256, 3, name, 0);
		cli();
		printf("created process pid=%d, parent_pid=%d\n", cpid, process_table->table[cpid]->ppid);
		sti();
	}
	cpid = start((void*)proc13, 256, 3, "proc_999", 0);
	cli();
	printf("created process pid=%d, parent_pid=%d\n", cpid, process_table->table[cpid]->ppid);
	sti();
	waitpid(-1, &retval);
	printf("child (pid %d) returned : %d\n", cpid, retval);
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
	printf("created process pid=%d\n",start((void*)proc1, 256, 1, "proc_1", 0));
	printf("created process pid=%d\n",start((void*)proc2, 256, 1, "proc_2", 0));

	idle();
	return;
}
