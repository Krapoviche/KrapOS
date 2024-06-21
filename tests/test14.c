#include "common.h"

/*******************************************************************************
 * Test 14
 *
 * Tests de preset et pdelete
 ******************************************************************************/
static int
psender1(void *arg)
{
	int fid1 = (int)arg;
	int fid2;
	int msg;

	printf(" 2");
	assert(preceive(fid1, &fid2) == 0);
	assert(psend(fid1, fid2) == 0);
	fid2 -= 42;
	assert(psend(fid1, 1) == 0);
	assert(psend(fid1, 2) == 0);
	assert(psend(fid1, 3) == 0);
	assert(psend(fid1, 4) == 0);
	assert(psend(fid1, 5) < 0);
	printf(" 6");
	assert(psend(fid1, 12) < 0);
	printf(" 9");
	assert(psend(fid1, 14) < 0);
	assert(preceive(fid2, &msg) < 0);
	printf(" 12");
	assert(preceive(fid2, &msg) < 0);
	assert(preceive(fid2, &msg) < 0);
	return 0;
}

static int
psender2(void *arg)
{
	int fid1 = (int)arg;
	int fid2;
	int msg;

	printf(" 3");
	assert(preceive(fid1, &fid2) == 0);
	fid2 -= 42;
	assert(psend(fid1, 6) < 0);
	printf(" 5");
	assert(psend(fid1, 7) == 0);
	assert(psend(fid1, 8) == 0);
	assert(psend(fid1, 9) == 0);
	assert(psend(fid1, 10) == 0);
	assert(psend(fid1, 11) < 0);
	printf(" 8");
	assert(psend(fid1, 13) < 0);
	assert((preceive(fid2, &msg) == 0) && (msg == 15));
	assert(preceive(fid2, &msg) < 0);
	printf(" 11");
	assert(preceive(fid2, &msg) < 0);
	assert(preceive(fid2, &msg) < 0);
	return 0;
}

static void
test(void)
{
	int pid1, pid2;
	int fid1 = pcreate(3);
	int fid2 = pcreate(3);
	int msg;

	/* Bravo si vous n'etes pas tombe dans le piege. */
	assert(pcreate(1073741827) < 0);

	printf("1");
	assert(getprio(getpid()) == 128);
	assert(fid1 >= 0);
	assert(psend(fid1, fid2 + 42) == 0);
	pid1 = start(psender1, 4000, 131, "psender1", (void *)fid1);
	pid2 = start(psender2, 4000, 130, "psender2", (void *)fid1);
	assert((preceive(fid1, &msg) == 0) && (msg == 1));
	assert(chprio(pid2, 132) == 130);
	printf(" 4");
	assert(preset(fid1) == 0);
	assert((preceive(fid1, &msg) == 0) && (msg == 7));
	printf(" 7");
	assert(pdelete(fid1) == 0);
	printf(" 10");
	assert(psend(fid2, 15) == 0);
	assert(preset(fid2) == 0);
	printf(" 13");
	assert(pdelete(fid2) == 0);
	assert(pdelete(fid2) < 0);
	assert(waitpid(pid2, 0) == pid2); //XXX assert(waitpid(-1, 0) == pid2); ???
	assert(waitpid(-1, 0) == pid1);
	printf(".\n");
}