#include "common.h"

/*******************************************************************************
 * Test 18
 *
 * Amusement : piratage !
 ******************************************************************************/
static char callhack[] = { 0xcd, 0x32, 0xc3 };

static int
dummy2(void *args)
{
	printf(" X");
	return (int)args;
}

__asm__(
".text\n"
".globl __hacking\n"
"__hacking:\n"

"pushal\n"
"pushl %ds\n"
"pushl %es\n"
"movl $0x18,%eax\n"
"movl %eax,%ds\n"
"movl %eax,%es\n"
"cld\n"
"call __hacking_c\n"
"popl %es\n"
"popl %ds\n"
"popal\n"
"iret\n"
);

extern void
__hacking(void);

static int
getpos(void)
{
	int pos;
	outb(0x0f, 0x3d4);
	pos = inb(0x3d4 + 1);
	outb(0x0e, 0x3d4);
	pos += inb(0x3d4 + 1) << 8;
	return pos;
}

static int firsttime = 1;

void
__hacking_c(void)
{
	static int pos;
	if (firsttime) {
		firsttime = 0;
		pos = getpos();
	} else {
		int pos2 = getpos();
		char *str = "          Kernel hacked ! :P          ";
		short *ptr = (short *)0xb8000;
		int p = pos;
		while (p > pos2) p-= 80;
		if ((p < 0) || (p >= 80 * 24)) p = 80 * 23;
		ptr += p;
		while (*str) {
			*ptr++ = ((128 + 4 * 16 + 15) << 8) + *str++;
		}
	}
}

static void
do_hack(void)
{
	firsttime = 1;
	((void (*)(void))callhack)();
	printf("nok.\n");
	((void (*)(void))callhack)();
}

static int
proc_18_1(void *args)
{
	printf("1 ");
	return (int)args;
}

static int
proc_18_2(void *args)
{
	printf("2 ");
	return (int)args;
}

static void
test(void)
{
	unsigned long a = (unsigned long)__hacking;
	unsigned long a1 = 0x100000 + (a & 0xffff);
	unsigned long a2 = (a & 0xffff0000) + 0xee00;
	int pid1, pid2;
	int cs;

	__asm__ volatile ("movl %%cs,%%eax":"=a" (cs));
	if ((cs & 3) == 0) {
		printf("This test can not work at kernel level.\n");
		return;
	}
	pid1 = start(proc_18_1, 4000, 127, "proc_18_1", (void *)a1);
	pid2 = start(proc_18_2, 4000, 126, "proc_18_2", (void *)a2);
	assert(pid1 > 0);
	assert(pid2 > 0);
	if ((waitpid(pid1, (int *)0x1190) == pid1) &&
		(waitpid(pid2, (int *)0x1194) == pid2)) {
		do_hack();
		return;
	}
	waitpid(-1, 0);
	waitpid(-1, 0);
	cons_write((char *)0x100000, 50);
	assert(start(dummy2, 4000, 100, (void *)0x100000, 0) < 0);
	printf("3.\n");
}