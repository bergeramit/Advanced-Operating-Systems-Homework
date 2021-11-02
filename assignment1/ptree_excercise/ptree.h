#include <include/uapi/asm-generic/errno-base.h>
#include <linux/kernel.h>
#include <include/linux/types.h>
#include <include/linux/kmod.h>
#include <include/linux/printk.h>


struct prinfo {
        pid_t parent_pid;       /* process id of parent */
        pid_t pid;              /* process id */
        long state;             /* current state of process */
        uid_t uid;              /* user id of process owner */
        char comm[16];          /* name of program executed */
        int level;              /* level of this process in the subtree */
    };

typedef int (*ptree_func)(struct prinfo *buf, int *nr, int pid);
int register_ptree(ptree_func func);
void unregister_ptree(ptree_func func);
