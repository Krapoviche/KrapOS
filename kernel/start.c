#include "debugger.h"
#include "cpu.h"
#include "screen.h"
#include "it.h"
#include "process.h"
#include "message.h"
#include "stdio.h"
#include "string.h"
#include "mem.h"
#include "start.h"

void idle(void){
	while(1){
		sti();
		hlt();
		cli();
	}
}

void* test_it49(int sn, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6){
	switch (sn){
		case SYS_WRITE:
			console_putbytes((char *)arg1,arg2);
			return 0;
		case SYS_MILLISLEEP:
			wait_clock(current_clock() + arg1*CLOCKFREQ/1000);
			return 0;
		case SYS_GETPID:
			return (void*)getpid();
		case SYS_START:
			return (void*)start((void*)arg1, (unsigned long)arg2, arg3, (const char*)arg4, (void*)arg5);
		case SYS_EXIT:
			exit(arg1);
			return 0;
		case SYS_KILL:
			return (void*)kill(arg1);
		case SYS_GETPPID:
			return (void*)getppid();
		case SYS_GETPRIORITY:
			return (void*)getprio(arg1);
		case SYS_SETPRIORITY:
			return (void*)chprio(arg1, arg2);
		case SYS_MQ_OPEN:
			return (void*)pcreate(arg1);
		case SYS_MQ_UNLINK:
			return (void*)pdelete(arg1);
		case SYS_MQ_SEND:
			return (void*)psend(arg1, arg2);
		case SYS_MQ_RECEIVE:
			return (void*)preceive(arg1, (int*)arg2);
		case SYS_MQ_RESET:
			return (void*)preset(arg1);
		case SYS_MQ_COUNT:
			return (void*)pcount(arg1, (int*)arg2);
		case SYS_WAITID:
			return (void*)waitpid(arg1, (int*)arg2);
		default:
			arg6 = arg6; // TODO: remove this line, it's just to avoid a warning
			return 0;
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
	printf("created process %d ", start(user_start, 4096, 1, "user_start", 0));

	idle();
	return;
}
