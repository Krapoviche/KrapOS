#include "libc.h"
#include "stdio.h"
void abc(int fid) {
    int ownpid = getpid();
    printf("%d GETPID\t\t= %d\n", ownpid, getpid());
    printf("%d SLEEP 2 SECS\n", ownpid);
    sleep(2);
    printf("%d PSEND(fid, 57)\n", ownpid);
    psend(fid, 57);
    exit(4);
}

void def(void) {
    while(1);
}

void user_start(void)
{
	// call_debugger(); useless with qemu -s -S
    int ownpid = getpid();
    int pid = ownpid;
    printf("%d GETPID\t\t= %d\n", ownpid, pid);
    printf("%d GETPRIO(getpid())\t= %d\n", ownpid, getprio(getpid()));
    printf("%d CHPRIO(getpid(), 2)\t= %d\n", ownpid, chprio(getpid(), 2));
    
    printf("%d CHPRIO(getpid(), 1)\t= %d\n", ownpid, chprio(getpid(), 1));
    int fid = pcreate(3);
    printf("%d PCREATE(3)\t\t= %d\n", ownpid, fid);
    printf("%d PSEND(fid, 42)\n", ownpid);
    psend(fid, 42);
    int msg;
    int ret = preceive(fid, &msg);
    printf("%d PRECEIVE(fid, &msg)\t= %d\tmsg = %d\n", ownpid, ret, msg);
    printf("%d PSEND(fid, 43)\t= %d\n", ownpid, psend(fid, 43));
    printf("%d PRESET(fid)\t\t= %d\n", ownpid, preset(fid));
    pid = start((void*)abc, 512, 0, "usr_child", (void*)fid);
    printf("%d START\t\t\t= %d\n", ownpid, pid);
    printf("%d SLEEP 1 SEC\n", ownpid);
    sleep(1);
    ret = preceive(fid, &msg);
    printf("%d PRECEIVE(fid, &msg)\t= %d\tmsg = %d\n", ownpid, ret, msg);
    printf("%d PDELETE(fid)\t\t= %d\n", ownpid, pdelete(fid));
    printf("%d PSEND(fid, 16)\t= %d < 0\n", ownpid, psend(fid, 16));
    printf("%d WAITPID(%d, &msg)\t= %d\t", ownpid, pid, waitpid(pid, &msg));
    printf("msg = %d\n", msg);
    pid = start((void*)def, 512, 0, "usr_child2", 0);
    printf("%d START\t\t\t= %d\n", ownpid, pid);
    printf("%d KILL(%d)\t\t= %d\n", ownpid, pid, kill(pid));
    printf("%d WAITPID(%d, &msg)\t= %d\t", ownpid, pid, waitpid(pid, &msg));
    printf("msg = %d\n", msg);

    while(1);

}
