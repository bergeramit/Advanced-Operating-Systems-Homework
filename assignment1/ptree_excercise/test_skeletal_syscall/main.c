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

void print_buf(struct prinfo buf[], int size)
{
	int i=0;
	printf("[\n");
	for(i=0; i<size; i++) {
		printf("\tparent_pid=%d, pid=%d, state=%ld, uid=%d, comm=%s, level=%d\n",
				buf[i].parent_pid, buf[i].pid, buf[i].state, buf[i].uid, buf[i].comm, buf[i].level);
	}
	printf("]\n");
}

int main(int argc, char* argv[]) {
	int nr=4;
	struct prinfo buf[4];
	pid_t target_pid;

	if (argc < 2) {
		printf("USAGE: ./test PID\n");
		return 0;
	}

	target_pid = (pid_t)atoi(argv[1]);

	printf("Print pid of: %d\n", target_pid);

	printf("calling syscall 449 - ptree from father\n");
	syscall(449, &buf, &nr, target_pid);
	print_buf(buf, nr);
        return 0;
}

