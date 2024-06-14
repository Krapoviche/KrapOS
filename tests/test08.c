#include "common.h"

/*******************************************************************************
 * Test 8
 *
 * Creation de processus se suicidant en boucle. Test de la vitesse de creation
 * de processus.
 ******************************************************************************/
static int
suicide(void *arg)
{
	(void)arg;
	kill(getpid());
	assert(0);
	return 0;
}

static int
suicide_launcher(void *arg)
{
	int pid1;
	(void)arg;
	pid1 = start(suicide, 4000, 192, "suicide_launcher", 0);
	assert(pid1 > 0);
	return pid1;
}

static void
test(void)
{
	unsigned long long tsc1;
	unsigned long long tsc2;
	int i, r, pid, count;

	assert(getprio(getpid()) == 128);

	/* Le petit-fils va passer zombie avant le fils mais ne pas
	etre attendu par waitpid. Un nettoyage automatique doit etre
	fait. */
	pid = start(suicide_launcher, 4000, 129, "suicide_launcher", 0);
	assert(pid > 0);
	assert(waitpid(pid, &r) == pid);
	assert(chprio(r, 192) < 0);

	count = 0;
	__asm__ __volatile__("rdtsc":"=A"(tsc1));
	do {
		for (i=0; i<10; i++) {
			pid = start(suicide_launcher, 4000, 200, "suicide_launcher", 0);
			assert(pid > 0);
			assert(waitpid(pid, 0) == pid);
		}
		test_it();
		count += i;
		__asm__ __volatile__("rdtsc":"=A"(tsc2));
	} while ((tsc2 - tsc1) < 1000000000);
	printf("%lu cycles/process.\n", (unsigned long)div64(tsc2 - tsc1, 2 * count, 0));
}