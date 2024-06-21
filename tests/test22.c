#include "common.h"

void abc(int sid) {
    int sw = swait(sid);
    if (140 - getprio(getpid()) <= 3) {
        assert(sw == -4);
    } else if (140 - getprio(getpid()) <= 6) {
        assert(sw == -3);
    } else {
        assert(sw == 0);
    }
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
    start((void*)abc, 256, 137, "abc_1", (void*)sid);
    start((void*)abc, 256, 138, "abc_2", (void*)sid);
    printf("1");
    sreset(sid, 0);
    waitpid(-1, 0);
    waitpid(-1, 0);
    start((void*)abc, 256, 134, "abc_3", (void*)sid);
    start((void*)abc, 256, 135, "abc_4", (void*)sid);
    printf(" 4");
    sdelete(sid);
    waitpid(-1, 0);
    waitpid(-1, 0);
    assert(swait(sid) == -1);
    sid = screate(2);
    start((void*)abc, 256, 133, "abc_5", (void*)sid);
    start((void*)abc, 256, 132, "abc_6", (void*)sid);
    waitpid(-1, 0);
    waitpid(-1, 0);
    start((void*)abc, 256, 129, "abc_7", (void*)sid);
    printf(" 9");
    start((void*)def, 256, 119, "def_1", (void*)sid);
    waitpid(-1, 0);
    waitpid(-1, 0);
    assert(try_wait(sid) == -3);
    printf(" 12.");
}