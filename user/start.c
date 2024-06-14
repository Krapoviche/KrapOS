#include "libc.h"
#include "stdio.h"
#include "../tests/test03.c"

void user_start(void)
{
    
    start((void*)test, 256, 128, "test", 0);

    while(1);

}
