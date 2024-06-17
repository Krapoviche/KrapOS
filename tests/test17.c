#include "common.h"

/*******************************************************************************
 * Test 17
 *
 * On teste des limites de capacite
 ******************************************************************************/
static int ids[1200];

static const int heap_len = 15 << 20;

static int
no_run(void *arg)
{
	(void)arg;
	assert(0);
	return 1;
}

static int
proc_return(void *arg)
{
	return (int)arg;
}

static void
test(void)
{
	int i, n, nx;
	int l = sizeof(ids) / sizeof(int);
	int count;
	int prio;
	unsigned long ssize;

	n = 0;
	while (1) {
		int fid = pcreate(1);
		if (fid < 0) break;
		ids[n++] = fid;
		if (n == l) {
			assert(!"Maximum number of queues too high !");
		}
		test_it();
	}
	for (i=0; i<n; i++) {
		assert(pdelete(ids[i]) == 0);
		test_it();
	}
	for (i=0; i<n; i++) {
		int fid = pcreate(1);
		assert(fid >= 0);
		ids[i] = fid;
		test_it();
	}
	assert(pcreate(1) < 0);
	for (i=0; i<n; i++) {
		assert(pdelete(ids[i]) == 0);
		test_it();
	}
	printf("%d", n);

	for (i=0; i<n; i++) {
		int fid = pcreate(1);
		assert(fid >= 0);
		assert(psend(fid, i) == 0);
		ids[i] = fid;
		test_it();
	}
	assert(pcreate(1) < 0);
	for (i=0; i<n; i++) {
		int msg;
		assert(preceive(ids[i], &msg) == 0);
		assert(msg == i);
		assert(pdelete(ids[i]) == 0);
		test_it();
	}

	count = heap_len / sizeof(int);
	count /= n - 1;
	nx = 0;
	while (nx < n) {
		int fid = pcreate(count);
		if (fid < 0) break;
		ids[nx++] = fid;
		test_it();
	}
	assert(nx < n);
	for (i=0; i<nx; i++) {
		assert(pdelete(ids[i]) == 0);
		test_it();
	}
	printf(" > %d", nx);

	prio = getprio(getpid());
	assert(prio == 128);
	n = 0;
	while (1) {
		int pid = start(no_run, 2000, 127, "no_run", 0);
		if (pid < 0) break;
		ids[n++] = pid;
		if (n == l) {
			assert(!"Maximum number of processes too high !");
		}
		test_it();
	}
	for (i=0; i<n; i++) {
		assert(kill(ids[i]) == 0);
		assert(waitpid(ids[i], 0) == ids[i]);
		test_it();
	}
	for (i=0; i<n; i++) {
		int pid = start(proc_return, 2000, 129, "return", (void *)i);
		assert(pid > 0);
		ids[i] = pid;
		test_it();
	}
	for (i=0; i<n; i++) {
		int retval;
		assert(waitpid(ids[i], &retval) == ids[i]);
		assert(retval == i);
		test_it();
	}
	printf(", %d", n);

	ssize = heap_len;
	ssize /= n - 1;
	nx = 0;
	while (nx < n) {
		int pid = start(proc_return, ssize, 127, "return", (void *)nx);
		if (pid < 0) break;
		ids[nx++] = pid;
		test_it();
	}
	assert(nx < n);
	for (i=0; i<nx; i++) {
		int retval;
		assert(waitpid(ids[i], &retval) == ids[i]);
		assert(retval == i);
		test_it();
	}
	printf(" > %d", nx);

	printf(".\n");
}