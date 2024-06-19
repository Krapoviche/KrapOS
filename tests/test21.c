#include "common.h"

void abc(int sid) {
    swait(sid);
    printf(" %d", 140-getprio(getpid()));
}
void def(int sid) {
    printf(" %d", 129-getprio(getpid()));
    signal(sid);
}

void test(void) {
    assert(getprio(getpid()) == 128);
    int sid = screate(0);
    assert(sid >= 0);
    start((void*)def, 256, 127, "def_1", (void*)sid); // 2
    printf("1");
    start((void*)abc, 256, 137, "abc_1", (void*)sid); // 3
    waitpid(-1, 0);
    waitpid(-1, 0);
    start((void*)def, 256, 125, "def_2", (void*)sid); // 4
    start((void*)abc, 256, 135, "abc_2", (void*)sid); // 5
    waitpid(-1, 0);
    waitpid(-1, 0);
    start((void*)abc, 256, 131, "abc_3", (void*)sid); // 9
    start((void*)abc, 256, 129, "abc_4", (void*)sid); // 11
    start((void*)abc, 256, 133, "abc_5", (void*)sid); // 7
    start((void*)def, 256, 121, "def_3", (void*)sid); // 8
    start((void*)def, 256, 119, "def_4", (void*)sid); // 10
    start((void*)def, 256, 123, "def_5", (void*)sid); // 6
    waitpid(-1, 0);
    waitpid(-1, 0);
    waitpid(-1, 0);
    waitpid(-1, 0);
    waitpid(-1, 0);
    waitpid(-1, 0);
    assert(try_wait(sid) == -3);
    printf(" 12.");
}