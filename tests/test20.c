#include "common.h"

/*******************************************************************************
 * Test 20
 *
 * Le repas des philosophes.
 ******************************************************************************/
static char f[NR_PHILO]; /* tableau des fourchettes, contient soit 1 soit 0 selon si elle
			    est utilisee ou non */

static char bloque[NR_PHILO]; /* memorise l'etat du philosophe, contient 1 ou 0 selon que le philosophe
				 est en attente d'une fourchette ou non */

struct sem {
	int fid;
};

static struct sem mutex_philo; /* exclusion mutuelle */
static struct sem s[NR_PHILO]; /* un semaphore par philosophe */
static int etat[NR_PHILO];

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

static void
affiche_etat()
{
	int i;
	printf("%c", 13);
	for (i=0; i<NR_PHILO; i++) {
		unsigned long c;
		switch (etat[i]) {
		case 'm':
			c = 2;
			break;
		default:
			c = 4;
		}
		int test = c %2 == 0;
		assert(test); // utilisation de c pour le compilo
		printf("%c", etat[i]);
	}
}

static void
waitloop(void)
{
	int j;
	for (j = 0; j < 5000; j++) {
		int l;
		test_it();
		for (l = 0; l < 5000; l++);
	}
}

static void
penser(long i)
{
	xwait(&mutex_philo); /* DEBUT SC */
	etat[i] = 'p';
	affiche_etat();
	xsignal(&mutex_philo); /* Fin SC */
	waitloop();
	xwait(&mutex_philo); /* DEBUT SC */
	etat[i] = '-';
	affiche_etat();
	xsignal(&mutex_philo); /* Fin SC */
}

static void
manger(long i)
{
	xwait(&mutex_philo); /* DEBUT SC */
	etat[i] = 'm';
	affiche_etat();
	xsignal(&mutex_philo); /* Fin SC */
	waitloop();
	xwait(&mutex_philo); /* DEBUT SC */
	etat[i] = '-';
	affiche_etat();
	xsignal(&mutex_philo); /* Fin SC */
}

static int
test_fork(int i)
{
	/* les fourchettes du philosophe i sont elles libres ? */
	return ((!f[i] && (!f[(i + 1) % NR_PHILO])));
}

static void
prendre_fourchettes(int i)
{
	/* le philosophe i prend des fourchettes */

	xwait(&mutex_philo); /* Debut SC */

	if (test_fork(i)) {  /* on tente de prendre 2 fourchette */
		f[i] = 1;
		f[(i + 1) % NR_PHILO] = 1;
		xsignal(&s[i]);
	} else
		bloque[i] = 1;

	xsignal(&mutex_philo); /* FIN SC */
	xwait(&s[i]); /* on attend au cas o on ne puisse pas prendre 2 fourchettes */
}

static void
poser_fourchettes(int i)
{

	xwait(&mutex_philo); /* DEBUT SC */

	if ((bloque[(i + NR_PHILO - 1) % NR_PHILO]) && (!f[(i + NR_PHILO - 1) % NR_PHILO])) {
		f[(i + NR_PHILO - 1) % NR_PHILO] = 1;
		bloque[(i + NR_PHILO - 1) % NR_PHILO] = 0;
		xsignal(&s[(i + NR_PHILO - 1) % NR_PHILO]);
	} else
		f[i] = 0;

	if ((bloque[(i + 1) % NR_PHILO]) && (!f[(i + 2) % NR_PHILO])) {
		f[(i + 2) % NR_PHILO] = 1;
		bloque[(i + 1) % NR_PHILO] = 0;
		xsignal(&s[(i + 1) % NR_PHILO]);
	} else
		f[(i + 1) % NR_PHILO] = 0;

	xsignal(&mutex_philo); /* Fin SC */
}

static int
philosophe(void *arg)
{
	/* comportement d'un seul philosophe */
	int i = (int) arg;
	int k;

	for (k = 0; k < 6; k++) {
		prendre_fourchettes(i); /* prend 2 fourchettes ou se bloque */
		manger(i); /* le philosophe mange */
		poser_fourchettes(i); /* pose 2 fourchettes */
		penser(i); /* le philosophe pense */
	}
	xwait(&mutex_philo); /* DEBUT SC */
	etat[i] = '-';
	affiche_etat();
	xsignal(&mutex_philo); /* Fin SC */
	return 0;
}

static int
launch_philo()
{

	int i, pid;

	for (i = 0; i < NR_PHILO; i++) {
		pid = start(philosophe, 4000, 192, "philosophe", (void *) i);
		assert(pid > 0);
	}
	return 0;

}

static void
test(void)
{
	int j, pid;

	xscreate(&mutex_philo); /* semaphore d'exclusion mutuelle */
	xsignal(&mutex_philo);
	for (j = 0; j < NR_PHILO; j++) {
		xscreate(s + j); /* semaphore de bloquage des philosophes */
		f[j] = 0;
		bloque[j] = 0;
		etat[j] = '-';
	}

	printf("\n");
	pid = start(launch_philo, 4000, 193, "Lanceur philosophes", 0);
	assert(pid > 0);
	assert(waitpid(pid, 0) == pid);
	printf("\n");
	xsdelete(&mutex_philo);
	for (j = 0; j < NR_PHILO; j++) {
		xsdelete(s + j);
	}
}