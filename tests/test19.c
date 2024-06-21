#include "common.h"

/*******************************************************************************
 * Test 19
 *
 * Quelques processus lisent sur la console et transmettent leur terminaison
 * via une file.
 ******************************************************************************/

struct sem {
	int fid;
};

static int
cons_reader(void *arg)
{
	int fid = (int)arg;
	char c;
	cons_read(&c, 1);
	assert(psend(fid, 1) == 0);
	printf(" %d (%c)", 134 - getprio(getpid()), c);
	return 0;
}

static void
test(void)
{
	int fid = pcreate(10);
	int pid1, pid2, pid3, pid4;
	int param;
	int count;

	assert(getprio(getpid()) == 128);
	assert(fid >= 0);
	cons_echo(0);
	pid1 = start(cons_reader, 4000, 130, "cons_reader", (void *)fid);
	assert(pid1 > 0);
	pid2 = start(cons_reader, 4000, 132, "cons_reader", (void *)fid);
	assert(pid2 > 0);
	pid3 = start(cons_reader, 4000, 131, "cons_reader", (void *)fid);
	assert(pid3 > 0);
	pid4 = start(cons_reader, 4000, 129, "cons_reader", (void *)fid);
	assert(pid4 > 0);
	printf("1");
	param = 4;
	while (param > 0) {
		unsigned long long t1, t2;
		int msg = 0;
		printf(".");
		__asm__ __volatile__("rdtsc":"=A"(t1));
		do {
			test_it();
			__asm__ __volatile__("rdtsc":"=A"(t2));
		} while ((t2 - t1) < 200000000);
		assert(psend(fid, 0) == 0);
		param++;
		do {
			assert(preceive(fid, &msg) == 0);
			param--;
		} while (msg);
	}
	assert(waitpid(pid2, 0) == pid2);
	assert(waitpid(pid3, 0) == pid3);
	assert(waitpid(pid1, 0) == pid1);
	assert(waitpid(pid4, 0) == pid4);
	cons_echo(1);
	assert(pcount(fid, &count) == 0);
	assert(count == 0);
	assert(pdelete(fid) == 0);
	printf(" 6.\n");
}