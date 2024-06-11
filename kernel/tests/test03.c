#include "common.h"

/*******************************************************************************
 * Test 3
 *
 * chprio() et ordre de scheduling
 * kill() d'un processus qui devient moins prioritaire
 ******************************************************************************/
static int
proc_prio4(void *arg)
{
	/* arg = priority of this proc. */
	int r;

	assert(getprio(getpid()) == (int) arg);
	printf(" 1");
	r = chprio(getpid(), 64);
	assert(r == (int) arg);
	printf(" 3");
	return 0;
}

static int
proc_prio5(void *arg)
{
	/* Arg = priority of this proc. */
	int r;

	assert(getprio(getpid()) == (int) arg);
	printf(" 7");
	r = chprio(getpid(), 64);
	assert(r == (int)arg);
	printf("error: I should have been killed\n");
	assert(0);
	return 0;
}

static void
test(void)
{
	int pid1;
	int p = 192;
	int r;

	assert(getprio(getpid()) == 128);
	pid1 = start(proc_prio4, 4000, p, "prio", (void *) p);
	assert(pid1 > 0);
	printf(" 2");
	r = chprio(getpid(), 32);
	assert(r == 128);
	printf(" 4");
	r = chprio(getpid(), 128);
	assert(r == 32);
	printf(" 5");
	assert(waitpid(pid1, 0) == pid1);
	printf(" 6");

	assert(getprio(getpid()) == 128);
	pid1 = start(proc_prio5, 4000, p, "prio", (void *) p);
	assert(pid1 > 0);
	printf(" 8");
	r = kill(pid1);
	assert(r == 0);
	assert(waitpid(pid1, 0) == pid1);
	printf(" 9");
	r = chprio(getpid(), 32);
	assert(r == 128);
	printf(" 10");
	r = chprio(getpid(), 128);
	assert(r == 32);
	printf(" 11.\n");
}
