#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>
#include <sys/mman.h>


int main(int argc, char* argv[]) {
	long int start = 0x11111111;
	long int end = 0x222222222;
	size_t out_size = 0;
	size_t max_size = 400;
	char buf[400] = {0};
	int N=5;
	void *ptr = NULL;

	ptr = mmap ( NULL, N*sizeof(int), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	if (ptr == MAP_FAILED) {
    	printf("Mapping Failed\n");
    	return 1;
	}
	printf("Mapped address: %p\n", ptr);

	printf("calling syscall 450 - mapspages\n");
	out_size = syscall(450, ptr, ptr + N*sizeof(int) + 100, buf, max_size);
	printf("Got out_size = %ld\n", out_size);

	printf("\n ----- Output ----- \n");
	printf("Got *buf = %s\n", buf);
    return 0;
}

