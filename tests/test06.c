#include "common.h"

/*******************************************************************************
 * Test 6
 *
 * Waitpid multiple.
 * Creation de processus avec differentes tailles de piles.
 ******************************************************************************/
extern int __proc6_1(void *arg);
extern int __proc6_2(void *arg);

__asm__(
	"    .text				\n"
	"    .globl __proc6_1	\n"
	"__proc6_1:				\n"
	"    movl $3,%eax		\n"
	"    ret				\n"
	"    					\n"
	"    .globl __proc6_2	\n"
	"__proc6_2:				\n"
	"    movl 4(%esp),%eax	\n"
	"    pushl %eax			\n"
	"    popl %eax			\n"
	"    ret				\n"
	".previous				\n"
);

static void
test(void)
{
	int pid1, pid2, pid3;
	int ret;

	assert(getprio(getpid()) == 128);
	pid1 = start(__proc6_1, 1000, 64, "proc6_1", 0);
	assert(pid1 > 0);
	pid2 = start(__proc6_2, 1004, 66, "proc6_2", (void*)4);
	assert(pid2 > 0);
	pid3 = start(__proc6_2, 0xffffffff, 65, "proc6_3", (void*)5);
	assert(pid3 < 0);
	pid3 = start(__proc6_2, 1008, 65, "proc6_3", (void*)5);
	assert(pid3 > 0);
	assert(waitpid(-1, &ret) == pid2);
	assert(ret == 4);
	assert(waitpid(-1, &ret) == pid3);
	assert(ret == 5);
	assert(waitpid(-1, &ret) == pid1);
	assert(ret == 3);
	assert(waitpid(pid1, 0) < 0);
	assert(waitpid(-1, 0) < 0);
	assert(waitpid(getpid(), 0) < 0);
	printf("ok.\n");
}
