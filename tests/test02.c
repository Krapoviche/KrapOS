#include "common.h"

/*******************************************************************************
 * Test 2
 *
 * kill() de fils suspendu pas demarre
 * waitpid() de ce fils termine par kill()
 * waitpid() de fils termine par exit()
 ******************************************************************************/
static int
dummy2(void *args)
{
	printf(" X");
	return (int)args;
}

static int
dummy2_2(void *args)
{
	printf(" 5");
	exit((int) args);
	assert(0);
	return 0;
}

static void
test(void)
{
	int rval;
	int r;
	int pid1;
	int val = 45;

	printf("1");
	pid1 = start(dummy2, 4000, 100, "procKill", (void *) val);
	assert(pid1 > 0);
	printf(" 2");
	r = kill(pid1);
	assert(r == 0);
	printf(" 3");
	r = waitpid(pid1, &rval);
	assert(rval == 0);
	assert(r == pid1);
	printf(" 4");
	pid1 = start(dummy2_2, 4000, 192, "procExit", (void *) val);
	assert(pid1 > 0);
	printf(" 6");
	r = waitpid(pid1, &rval);
	assert(rval == val);
	assert(r == pid1);
	assert(waitpid(getpid(), &rval) < 0);
	printf(" 7.\n");
}