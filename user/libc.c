#include "libc.h"

void sleep(unsigned long secs) {
    millisleep(secs * 1000);
}

int signal(int sid) {
    return signaln(sid, 1);
}

int wait(int* retvalp) {
    return waitpid(-1, retvalp);
}

void console_putbytes(const char *s, int len) {
    cons_write(s, len);
}