#include "libc.h"

void sleep(unsigned long secs) {
    millisleep(secs * 1000);
}

int wait(int* retvalp) {
    return waitpid(-1, retvalp);
}