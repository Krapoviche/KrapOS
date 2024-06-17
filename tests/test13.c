#include "common.h"

/*******************************************************************************
 * Test 13
 *
 * Teste l'ordre entre les processus emetteurs et recepteurs sur une file.
 * Teste le changement de priorite d'un processus bloque sur une file.
 ******************************************************************************/
struct psender {
	int fid;
	const char *data;
};

static int
psender(void *arg)
{
	struct psender *ps = arg;
	int i;
	int n = strlen(ps->data);

	for(i=0; i<n; i++) {
		assert(psend(ps->fid, ps->data[i]) == 0);
	}
	return 0;
}

static int
preceiver(void *arg)
{
	struct psender *ps = arg;
	int i, msg;
	int n = strlen(ps->data);

	for(i=0; i<n; i++) {
		assert(preceive(ps->fid, &msg) == 0);
		assert(msg == ps->data[i]);
	}
	return 0;
}

static void
test(void)
{
	struct psender ps1, ps2, ps3;
	int pid1, pid2, pid3;
	int fid = pcreate(3);
	int i, msg;

	printf("1");
	assert(getprio(getpid()) == 128);
	assert(fid >= 0);
	ps1.fid = ps2.fid = ps3.fid = fid;
	ps1.data = "abcdehm";
	ps2.data = "il";
	ps3.data = "fgjk";
	pid1 = start(psender, 4000, 131, "psender1", &ps1);
	pid2 = start(psender, 4000, 130, "psender2", &ps2);
	pid3 = start(psender, 4000, 129, "psender3", &ps3);
	for (i=0; i<2; i++) {
		assert(preceive(fid, &msg) == 0);
		assert(msg == 'a' + i);
	}
	chprio(pid1, 129);
	chprio(pid3, 131);
	for (i=0; i<2; i++) {
		assert(preceive(fid, &msg) == 0);
		assert(msg == 'c' + i);
	}
	chprio(pid1, 127);
	chprio(pid2, 126);
	chprio(pid3, 125);
	for (i=0; i<6; i++) {
		assert(preceive(fid, &msg) == 0);
		assert(msg == 'e' + i);
	}
	chprio(pid1, 125);
	chprio(pid3, 127);
	for (i=0; i<3; i++) {
		assert(preceive(fid, &msg) == 0);
		assert(msg == 'k' + i);
	}
	assert(waitpid(pid3, 0) == pid3); //XXX assert(waitpid(-1, 0) == pid3); ???
	assert(waitpid(-1, 0) == pid2);
	assert(waitpid(-1, 0) == pid1);
	printf(" 2");

	ps1.data = "abej";
	ps2.data = "fi";
	ps3.data = "cdgh";
	pid1 = start(preceiver, 4000, 131, "preceiver1", &ps1);
	pid2 = start(preceiver, 4000, 130, "preceiver2", &ps2);
	pid3 = start(preceiver, 4000, 129, "preceiver3", &ps3);
	for (i='a'; i<='b'; i++) {
		assert(psend(fid, i) == 0);
	}
	chprio(pid1, 129);
	chprio(pid3, 131);
	for (i='c'; i<='d'; i++) {
		assert(psend(fid, i) == 0);
	}
	chprio(pid1, 127);
	chprio(pid2, 126);
	chprio(pid3, 125);
	for (i='e'; i<='j'; i++) {
		assert(psend(fid, i) == 0);
	}
	chprio(pid1, 125);
	chprio(pid3, 127);
	assert(waitpid(-1, 0) == pid3);
	assert(waitpid(-1, 0) == pid2);
	assert(waitpid(-1, 0) == pid1);
	assert(pdelete(fid) == 0);
	printf(" 3.\n");
}