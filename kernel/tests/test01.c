#include "common.h"

/*******************************************************************************
 * Test 1
 *
 * Demarrage de processus avec passage de parametre
 * Terminaison normale avec valeur de retour
 * Attente de terminaison (cas fils avant pere et cas pere avant fils)
 ******************************************************************************/
static int
dummy1(void *arg)
{
	printf("1");
	assert((int) arg == DUMMY_VAL);
	return 3;
}

static int
dummy1_2(void *arg)
{
	printf(" 5");
	assert((int) arg == DUMMY_VAL + 1);

	return 4;
}

static void
test(void)
{
	int pid1;
	int r;
	int rval;

	pid1 = start(dummy1, 4000, 192, "paramRetour", (void *) DUMMY_VAL);
	assert(pid1 > 0);
	printf(" 2");
	r = waitpid(pid1, &rval);
	assert(r == pid1);
	assert(rval == 3);
	printf(" 3");
	pid1 = start(dummy1_2, 4000, 100, "paramRetour", (void *) (DUMMY_VAL + 1));
	assert(pid1 > 0);
	printf(" 4");
	r = waitpid(pid1, &rval);
	assert(r == pid1);
	assert(rval == 4);
	printf(" 6.\n");
}
