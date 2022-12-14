#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sched.h>
#include <signal.h>
#include <unistd.h>
 
#define STACK_SIZE (1024 * 1024)
static char container_stack[STACK_SIZE];
 
char* const container_args[] = {
    "/bin/bash",
    NULL
};
 
int container_main(void* arg)
{
    printf("Container[%5d] - inside the container!\n", getpid());
    sethostname("container",10); 
    /* 重新mount proc文件系统到 /proc下 */
    // system("mount -t proc proc /proc");
    system("mount --bind /home/uos/Desktop/mount_dir /mnt");
    int ret = execv(container_args[0], container_args);
    printf("execv cmd ret:%d!\n", ret);
    return 1;
}
 
int main()
{
    printf("Parent[%5d] - start a container!\n", getpid());
    int container_pid = clone(container_main, container_stack+STACK_SIZE, 
                              SIGCHLD | CLONE_NEWUTS | CLONE_NEWIPC | CLONE_NEWPID | CLONE_NEWNS
                              , NULL);
    waitpid(container_pid, NULL, 0);
    printf("Parent - container stopped!\n");
    return 0;
}
