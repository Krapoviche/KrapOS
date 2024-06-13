#include "debugger.h"
#include "cpu.h"
#include "screen.h"
#include "it.h"
#include "process.h"
#include "message.h"
#include "stdio.h"
#include "string.h"
#include "mem.h"

void idle(void){
	while(1){
		sti();
		hlt();
		cli();
	}
}

void* test_it49(int sn, int arg2, int arg3){
	if(sn == 0){
		printf("SOFTWARE INTERUPTED WITH PARAM 1 %d\n",sn);
		printf("GETPID RETURNS %d\n", getpid());
		return((void*)getpid());
	} else if (sn == 1) {
		printf("SOFTWARE INTERUPTED WITH PARAM 1 %d\n",sn);		
		printf("SOFTWARE INTERUPTED WITH PARAM 2 %s\n",(char*)arg2);
		console_putbytes((char *)arg2,arg3);
		return 0;
	} else if (sn == 2){
		printf("SOFTWARE INTERUPTED WITH PARAM 1 %d\n",sn);
		printf("SOFTWARE INTERUPTED WITH PARAM 2 %d\n",arg2);
		return 0;
	} else {return 0;}
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
