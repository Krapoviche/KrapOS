#include "stdio.h"
#include "string.h"
#include "libc.h"
#include "tests/TEST_CASE"

void user_start(void)
{
	// call_debugger(); useless with qemu -s -S


	start((void*)test, 256, 128, "test", 0);

    while(1){};

	return;
}
