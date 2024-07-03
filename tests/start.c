#include "stdio.h"
#include "string.h"
#include "libc.h"
#include "shell.h"
#include "tests/TEST_CASE"

void user_start(void)
{
	// call_debugger(); useless with qemu -s -S

	start((void*)shell, 256, 128, "shell", 0);
	start((void*)test, 256, 128, "test", 0);

    while(1){};

	return;
}
