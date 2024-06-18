#include "libc.h"
#include "stdio.h"
#include "shell.h"

void user_start(void)
{
    start((void*)shell, 4096, 32, "shell_1", (void*)true);
    while(1);

}
