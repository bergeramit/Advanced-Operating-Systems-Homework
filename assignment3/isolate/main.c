#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdint.h>
#include <errno.h>

#define STACK_SIZE (1024*1024)
char stack_child[STACK_SIZE];

struct child_args {
    int fd[2];
    char **argv;
};

void setup_mntns() {
    system("mount -t proc proc /proc");
}

void setup_utsns() {
    sethostname("isolated", 8);
}

void setup_userns(pid_t child_pid) {
    char uid_path[200] = {0};
    char gid_path[200] = {0};
    char setgroups_path[200] = {0};
    int uid_fd, gid_fd, rc, setgroups_fd;

    // printf("Child's PID: %d\n", child_pid);
    snprintf(setgroups_path, 200, "/proc/%d/setgroups", child_pid);
    sprintf(uid_path, "/proc/%d/uid_map", child_pid);
    sprintf(gid_path, "/proc/%d/gid_map", child_pid);

    uid_fd = open(uid_path, O_RDWR);
    gid_fd = open(gid_path, O_RDWR);
    setgroups_fd = open(setgroups_path, O_RDWR);

    write(uid_fd, "0 1000 1000", 12);
    write(setgroups_fd, "deny", 5);
    write(gid_fd, "0 1000 1000", 12);

    close(uid_fd);
    close(gid_fd);
    close(setgroups_fd);

}

void setup_netns(pid_t child_pid) {
    char veth_add_command[200] = {0};

    system("ip link add veth0 type veth peer name peer0");
    system("ip link set veth0 up");
    system("ip addr add 10.11.12.13/24 dev veth0");
    sprintf(veth_add_command, "ip link set peer0 netns /proc/%d/ns/net", child_pid);
    system(veth_add_command);
}

int child_func(void *args)
{
    struct child_args *current_args = (struct child_args *)args;
    pid_t pid;

    read(current_args->fd[0], &pid, sizeof(int));
    close(current_args->fd[0]);

    setgid(0);
    setuid(0);

    setup_mntns();
    setup_utsns();

    execvp(current_args->argv[0], current_args->argv);
}

int main(int argc, char *argv[]) {
    int fds[2];
    int flags;
    pid_t pid;
    struct child_args child_args = {0};

    if (argc <= 1) {
        printf("Usage: isolate PROGRAM [ARGS...]\n");
    }

    child_args.argv = &argv[1];
    pipe(child_args.fd);

    // recieve signal on child process termination
    flags = SIGCHLD | \
            CLONE_NEWUSER | CLONE_NEWNET | CLONE_NEWUTS | \
            CLONE_NEWIPC | CLONE_NEWNS | CLONE_NEWPID;

    // the child stack growns downwards 
    pid = clone(child_func, stack_child + STACK_SIZE, flags, &child_args);
    if (pid == -1) {
        fprintf(stderr,"clone: %s", strerror(errno));
        exit(1);
    }

    setup_userns(pid);
    setup_netns(pid);
    // printf("Done setting setup_userns + setup_netns\n");
    
    write(child_args.fd[1], &pid, sizeof(int));
    close(child_args.fd[1]);
    waitpid(pid, NULL, 0);
}