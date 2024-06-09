#include "common.h"

/*******************************************************************************
 * Test 9
 *
 * Test de la sauvegarde des registres dans les appels systeme et interruptions
 ******************************************************************************/

static int
nothing(void *arg)
{
	(void)arg;
	return 0;
}

int __err_id = 0;

extern void
__test_error(void)
{
	(void)nothing;
	printf("assembly check failed, id = %d\n", __err_id);
	exit(1);
}

__asm__(
".text\n"
".globl __test_valid_regs1\n"
"__test_valid_regs1:\n"

"pushl %ebp; movl %esp, %ebp; pushal\n"
"movl 8(%ebp),%ebx\n"
"movl 12(%ebp),%edi\n"
"movl 16(%ebp),%esi\n"
"movl %ebp,1f\n"
"movl %esp,2f\n"

"pushl $0\n"
"pushl $3f\n"
"pushl $192\n"
"pushl $4000\n"
"pushl $nothing\n"
"call start\n"
"addl $20,%esp\n"
"movl $1,__err_id\n"
"testl %eax,%eax\n"
"jle 0f\n"
"pushl %eax\n"
"pushl $0\n"
"pushl %eax\n"
"call waitpid\n"
"addl $8,%esp\n"
"popl %ecx\n"
"movl $3,__err_id\n"
"cmpl %ecx,%eax\n"
"jne 0f\n"

"movl $4,__err_id\n"
"cmpl %esp,2f\n"
"jne 0f\n"
"movl $5,__err_id\n"
"cmpl %ebp,1f\n"
"jne 0f\n"
"movl $6,__err_id\n"
"cmpl 8(%ebp),%ebx\n"
"jne 0f\n"
"movl $7,__err_id\n"
"cmpl 12(%ebp),%edi\n"
"jne 0f\n"
"movl $8,__err_id\n"
"cmpl 16(%ebp),%esi\n"
"jne 0f\n"
"popal; leave\n"
"ret\n"
"0: jmp __test_error\n"
"0:\n"
"jmp 0b\n"
".previous\n"
".data\n"
"1: .long 0x12345678\n"
"2: .long 0x87654321\n"
"3: .string \"nothing\"\n"
".previous\n"
);

extern void
__test_valid_regs1(int a1, int a2, int a3);

__asm__(
".text\n"
".globl __test_valid_regs2\n"
"__test_valid_regs2:\n"

"pushl %ebp; movl %esp, %ebp; pushal\n"

"movl 8(%ebp),%eax\n"
"movl 12(%ebp),%ebx\n"
"movl 16(%ebp),%ecx\n"
"movl 20(%ebp),%edx\n"
"movl 24(%ebp),%edi\n"
"movl 28(%ebp),%esi\n"
"movl %ebp,1f\n"
"movl %esp,2f\n"

"movl $1,__err_id\n"
"3: testl $1,__it_ok\n"
"jnz 0f\n"

"3: pushfl\n"
"testl $0x200,(%esp)\n"
"jnz 4f\n"
"sti\n"
"nop\n"
"cli\n"
"4:\n"
"addl $4,%esp\n"
"testl $1,__it_ok\n"
"jz 3b\n"

"movl $2,__err_id\n"
"cmpl %esp,2f\n"
"jne 0f\n"
"movl $3,__err_id\n"
"cmpl %ebp,1f\n"
"jne 0f\n"
"movl $4,__err_id\n"
"cmpl 8(%ebp),%eax\n"
"jne 0f\n"
"movl $5,__err_id\n"
"cmpl 12(%ebp),%ebx\n"
"jne 0f\n"
"movl $6,__err_id\n"
"cmpl 16(%ebp),%ecx\n"
"jne 0f\n"
"movl $7,__err_id\n"
"cmpl 20(%ebp),%edx\n"
"jne 0f\n"
"movl $8,__err_id\n"
"cmpl 24(%ebp),%edi\n"
"jne 0f\n"
"movl $9,__err_id\n"
"cmpl 28(%ebp),%esi\n"
"jne 0f\n"
"popal; leave\n"
"ret\n"
"0: jmp __test_error\n"
"0:\n"
"jmp 0b\n"
".previous\n"
".data\n"
"1: .long 0x12345678\n"
"2: .long 0x87654321\n"
".previous\n"
);

static volatile int __it_ok;

extern void
__test_valid_regs2(int a1, int a2, int a3, int a4, int a5, int a6);

static int
test_regs2(void *arg)
{
	(void)arg;
	__it_ok = 0;
	__test_valid_regs2(rand(), rand(), rand(), rand(), rand(), rand());
	return 0;
}

static void
test(void)
{
	int i;
	assert(getprio(getpid()) == 128);
	printf("1");
	for (i=0; i<1000; i++) {
		__test_valid_regs1(rand(), rand(), rand());
	}
	printf(" 2");
	for (i=0; i<25; i++) {
		int pid;
		__it_ok = 1;
		pid = start(test_regs2, 4000, 128, "test_regs2", 0);
		assert(pid > 0);
		while (__it_ok) test_it();
		__it_ok = 1;
		assert(waitpid(pid, 0) == pid);
	}
	printf(" 3.\n");
}
