#include "ptree.h"

ptree_func get_ptree_g = NULL;

int register_ptree(ptree_func func)
{
	if (get_ptree_g == NULL) {
		get_ptree_g = func;
		return 0;

	} else {
		return -EBUSY;
	}

}

void unregister_ptree(ptree_func func)
{
	if (get_ptree_g == func) {
		get_ptree_g = NULL;
	}
}

asmlinkage int sys_ptree(struct prinfo *buf, int *nr, int pid)
{
	int rc = -1;
	int _nr = NULL;
	int bytes_copied = 0;
	struct prinfo *_buf = NULL;

	pr_info("entered syscall ptree\n");
	if (get_ptree_g == NULL) {
		rc = request_module("/data/ubuntu/linux/ptree_module.ko");
		if (rc != 0) {
			pr_crit("Failed to request_module\n");
			rc = -ENOSYS;
			goto Exit;
		}
	}
	pr_info("Request module success\n");
	bytes_copied = copy_from_user(&_nr, nr, sizeof(int));
	if (bytes_copied != sizeof(int)) {
		pr_crit("Failed to copy_from_user nr\n");
		rc = -ENOSYS;
		goto Exit;
	}

	_buf = (struct prinfo*)kmalloc(_nr * sizeof(struct prinfo));
	if (_buf == NULL) {
		pr_crit("Failed to kmalloc\n");
		rc = -ENOSYS;
		goto Exit;
	}

	pr_info("Calling get_ptree_g \n");
	rc = get_ptree_g(_buf, &_nr, pid);
	if (rc != 0) {
		goto Exit;
	}

	pr_info("Return success from get_ptree_g\n");
	bytes_copied = copy_to_user(buf, _buf, _nr * sizeof(struct prinfo));
	if (bytes_copied != (_nr * sizeof(struct prinfo))) {
		pr_crit("Failed to copy_to_user buf\n");
		rc = -ENOSYS;
		goto Exit;
	}

	bytes_copied = copy_to_user(nr, &_nr, sizeof(int));
	if (bytes_copied != sizeof(int)) {
		pr_crit("Failed to copy_to_user nr\n");
		rc = -ENOSYS;
		goto Exit;
	}
	pr_info("Finished copy to user, exiting\n");

Exit:
	return rc;
}

