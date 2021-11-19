#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>
#include <sys/mman.h>


int main(int argc, char* argv[]) {
	size_t out_size = 0;
	size_t max_size = 400;
	char buf[400] = {0};
	int N=5000;
	void *ptr = NULL;

	ptr = mmap ( NULL, N*sizeof(int), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	if (ptr == MAP_FAILED) {
    	printf("Mapping Failed\n");
    	return 1;
	}
	printf("Mapped address: %p\n", ptr);

	printf("Access: first and last page...\n");
	*(int *)ptr = 10;
	*(int *)(ptr + (N-1)*sizeof(int)) = 10;

	printf("calling syscall 450 - mapspages\n");
	out_size = syscall(450, ptr, ptr + N*sizeof(int), buf, max_size);

	printf("\n ----- Output ----- \n");
	printf("%s\n", buf);
    return 0;
}
