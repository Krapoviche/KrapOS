#include "syscall.h"
#include "it.h"
#include "primitive.h"
#include "process.h"
#include "kbd.h"
#include "ps.h"

void* test_it49(int sn, int arg1, int arg2, int arg3, int arg4, int arg5) {
	switch (sn){
		case SYS_CLOCK_GETTIME:
			return (void *)current_clock();
		case SYS_CLOCK_GETRES:
			clock_settings((unsigned long *) arg1,(unsigned long *) arg2);
			return 0;
		case SYS_WAITCLOCK:
			wait_clock(arg1);
			return 0;
		case SYS_MILLISLEEP:
			wait_clock(current_clock() + arg1*CLOCKFREQ/1000);
			return 0;
		case SYS_GETPID:
			return (void*)getpid();
		case SYS_START:
			if (!is_user_address((uint32_t)arg4)) { return (void*)-1; }
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
			if (!is_user_address((uint32_t)arg2)) { return (void*)-1; }
			return (void*)waitpid(arg1, (int*)arg2);
		case SYS_CONSREAD:
			return (void *)cons_read((char *)arg1, arg2);
		case SYS_CONSWRITE:
			if (!is_user_address((uint32_t)arg1)) { return (void*)-1; }
			cons_write((const char *)arg1, (long)arg2);
			return 0;
		case SYS_CONSECHO:
			cons_echo(arg1);
			return 0;
		case SYS_PS:
			ps();
			return 0;
		default:
			return (void*)-1;
	}
}