#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>

struct prinfo {
        pid_t parent_pid;       /* process id of parent */
        pid_t pid;              /* process id */
        long state;             /* current state of process */
        uid_t uid;              /* user id of process owner */
        char comm[16];          /* name of program executed */
        int level;              /* level of this process in the subtree */
};

int main(int argc, char* argv[]) {
	int nr = 1;
        pid_t my_pid = getpid();
	struct prinfo buf = {0};
	printf("Trying to call syscall skeletal without .ko library\n");
	syscall(449, &buf, &nr, my_pid);
        return 0;
}

