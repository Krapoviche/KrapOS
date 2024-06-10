#include "common.h"

/*******************************************************************************
 * Test 16
 *
 * Test sur des files de diverses tailles et test d'endurance
 ******************************************************************************/
struct tst16 {
	int count;
	int fid;
};

static int
proc_16_1(void *arg)
{
	struct tst16 *p = arg;
	int i, msg;
	for (i=0; i<=p->count; i++) {
		assert(preceive(p->fid, &msg) == 0);
		assert(msg == i);
		test_it();
	}
	return 0;
}

static int
proc_16_2(void *arg)
{
	struct tst16 *p = arg;
	int i, msg;
	for (i=0; i<p->count; i++) {
		assert(preceive(p->fid, &msg) == 0);
		test_it();
	}
	return 0;
}

static int
proc_16_3(void *arg)
{
	struct tst16 *p = arg;
	int i;
	for (i=0; i<p->count; i++) {
		assert(psend(p->fid, i) == 0);
		test_it();
	}
	return 0;
}

static void
test(void)
{
	int i, count, fid, pid;
	struct tst16 p;
	int procs = 10;
	int pids[2*procs];

	assert(getprio(getpid()) == 128);
	for (count=1; count<=100; count++) {
		fid = pcreate(count);
		assert(fid >= 0);
		p.count = count;
		p.fid = fid;
		pid = start(proc_16_1, 2000, 128, "proc_16_1", &p);
		assert(pid > 0);
		for (i=0; i<=count; i++) {
			assert(psend(fid, i) == 0);
			test_it();
		}
		assert(waitpid(pid, 0) == pid);
		assert(pdelete(fid) == 0);
	}

	p.count = 20000;
	fid = pcreate(50);
	assert(fid >= 0);
	p.fid = fid;
	for (i=0; i<procs; i++) {
		pid = start(proc_16_2, 2000, 127, "proc_16_2", &p);
		assert(pid > 0);
		pids[i] = pid;
	}
	for (i=0; i<procs; i++) {
		pid = start(proc_16_3, 2000, 127, "proc_16_3", &p);
		assert(pid > 0);
		pids[procs + i] = pid;
	}
	for (i=0; i<2*procs; i++) {
		assert(waitpid(pids[i], 0) == pids[i]);
	}
	assert(pcount(fid, &count) == 0);
	assert(count == 0);
	assert(pdelete(fid) == 0);
	printf("ok.\n");
}