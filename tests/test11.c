#include "common.h"

/*******************************************************************************
 * Test 11
 *
 * Mutex avec un semaphore, regle de priorite sur le mutex.
 ******************************************************************************/
struct sem {
	int fid;
};

static void
xwait(struct sem *s)
{
	assert(preceive(s->fid, 0) == 0);
}

static void
xsignal(struct sem *s)
{
	int count;
	assert(psend(s->fid, 1) == 0);
	assert(pcount(s->fid, &count) == 0);
	//assert(count == 1); XXX
	assert(count < 2);
}

static void
xscreate(struct sem *s)
{
	assert((s->fid = pcreate(2)) >= 0);
}

static void
xsdelete(struct sem *s)
{
	assert(pdelete(s->fid) == 0);
}

static int in_mutex = 0;

static int
proc_mutex(void *arg)
{
	struct sem *sem = arg;
	int p = getprio(getpid());
	int msg;

	switch (p) {
	case 130:
		msg = 2;
		break;
	case 132:
		msg = 3;
		break;
	case 131:
		msg = 4;
		break;
	case 129:
		msg = 5;
		break;
	default:
		msg = 15;
	}
	printf(" %d", msg);
	xwait(sem);
	printf(" %d", 139 - p);
	assert(!(in_mutex++));
	chprio(getpid(), 16);
	chprio(getpid(), p);
	in_mutex--;
	xsignal(sem);
	return 0;
}

static void
test(void)
{
	struct sem sem;
	int pid1, pid2, pid3, pid4;

	assert(getprio(getpid()) == 128);
	xscreate(&sem);
	printf("1");
	pid1 = start(proc_mutex, 4000, 130, "proc_mutex", &sem);
	pid2 = start(proc_mutex, 4000, 132, "proc_mutex", &sem);
	pid3 = start(proc_mutex, 4000, 131, "proc_mutex", &sem);
	pid4 = start(proc_mutex, 4000, 129, "proc_mutex", &sem);
	assert(pid1 > 0);
	assert(pid2 > 0);
	assert(pid3 > 0);
	assert(pid4 > 0);
	assert(chprio(getpid(), 160) == 128);
	printf(" 6");
	xsignal(&sem);
	assert(waitpid(-1, 0) == pid2);
	assert(waitpid(-1, 0) == pid3);
	assert(waitpid(-1, 0) == pid1);
	assert(waitpid(-1, 0) == pid4);
	assert(waitpid(-1, 0) < 0);
	assert(chprio(getpid(), 128) == 160);
	xsdelete(&sem);
	printf(" 11.\n");
}