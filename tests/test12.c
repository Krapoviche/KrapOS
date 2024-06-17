#include "common.h"

/*******************************************************************************
 * Test 12
 *
 * Tests de rendez-vous sur une file de taille 1.
 ******************************************************************************/
static int
rdv_proc(void *arg)
{
	int fid = (int) arg;
	int msg;
	int count;

	printf(" 2");
	assert(psend(fid, 3) == 0); /* Depose dans le tampon */
	printf(" 3");
	assert((pcount(fid, &count) == 0) && (count == 1));
	assert(psend(fid, 4) == 0); /* Bloque tampon plein */
	printf(" 5");
	assert((pcount(fid, &count) == 0) && (count == 1));
	assert(preceive(fid, &msg) == 0); /* Retire du tampon */
	assert(msg == 4);
	printf(" 6");
	assert(preceive(fid, &msg) == 0); /* Bloque tampon vide. */
	assert(msg == 5);
	printf(" 8");
	assert((pcount(fid, &count) == 0) && (count == 0));
	return 0;
}

static void
test(void)
{
	int fid;
	int pid;
	int msg;
	int count;

	assert(getprio(getpid()) == 128);
	assert((fid = pcreate(1)) >= 0);
	printf("1");
	pid = start(rdv_proc, 4000, 130, "rdv_proc", (void *)fid);
	assert(pid > 0);
	printf(" 4");
	assert((pcount(fid, &count) == 0) && (count == 2));
	assert(preceive(fid, &msg) == 0); /* Retire du tampon et debloque un emetteur. */
	assert(msg == 3);
	printf(" 7");
	assert((pcount(fid, &count) == 0) && (count == -1));
	assert(psend(fid, 5) == 0); /* Pose dans le tampon. */
	printf(" 9");
	assert(psend(fid, 6) == 0); /* Pose dans le tampon. */
	assert(preceive(fid, &msg) == 0); /* Retire du tampon. */
	assert(msg == 6);
	assert(pdelete(fid) == 0);
	assert(psend(fid, 2) < 0);
	assert(preceive(fid, &msg) < 0);
	assert(waitpid(-1, 0) == pid);
	printf(" 10.\n");
}