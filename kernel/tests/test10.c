#include "common.h"

/*******************************************************************************
 * Test 10
 *
 * Test d'utilisation d'une file comme espace de stockage temporaire.
 ******************************************************************************/

static void write(int fid, const char *buf, unsigned long len)
{
	unsigned long i;
	for (i=0; i<len; i++) {
		assert(psend(fid, buf[i]) == 0);
	}
}

static void read(int fid, char *buf, unsigned long len)
{
	unsigned long i;
	for (i=0; i<len; i++) {
		int msg;
		assert(preceive(fid, &msg) == 0);
		buf[i] = msg;
	}
}

static void
test(void)
{
	int fid;
	char *str = "abcde";
	unsigned long len = strlen(str);
	char buf[10];

	printf("1");
	assert((fid = pcreate(5)) >= 0);
	write(fid, str, len);
	printf(" 2");
	read(fid, buf, len);
	buf[len] = 0;
	assert(strcmp(str, buf) == 0);
	assert(pdelete(fid) == 0);
	printf(" 3.\n");
}
