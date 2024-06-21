#include "libc.h"
#include "stdio.h"
#include "shell.h"

void user_start(void)
{
    start((void*)shell, 8192, 32, "shell", (void*)true);
    exit(0);
}
