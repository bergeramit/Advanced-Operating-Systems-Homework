#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>
#include <sys/mman.h>
#include "mapspages_tests.h"

#define TEST_FUNC_IDX	0
#define TEST_STATUS_IDX	1

int main(int argc, char* argv[]) {
	testfunc tests[MAPSPAGES_TESTS_NUM] = {
		test1,
		test2,
		test3,
		test4,
		test5,
		test6,
		test8,
		test9
		};
	int tests_stats[MAPSPAGES_TESTS_NUM] = {0};
	int ret = 0;

	printf("\n ----- Starting all tests ----- \n");
	for (int i = 0; i < MAPSPAGES_TESTS_NUM; i++) {
		printf("Starting test %d...\n\n", i + 1);
		ret = tests[i]();
		tests_stats[i] = ret;
		if (ret != 0) {
			// printf("Test failed with return value: %d\n", ret);
			break;
		}
	}

	for (int i = 0; i < MAPSPAGES_TESTS_NUM; i++) {
		printf("Test %d finished with retval: %d (%s)\n",
			i + 1,
			tests_stats[i],
			tests_stats[i] == 0 ? "SUCCESS" : "FAILED");
		if (tests_stats[i] != 0) {
			break;
		}
	}

	// size_t out_size = 0;
	// size_t max_size = 400;
	// char buf[400] = {0};
	// int N=5000;
	// void *ptr = NULL;

	// ptr = mmap ( NULL, N*sizeof(int), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	// if (ptr == MAP_FAILED) {
    // 	printf("Mapping Failed\n");
    // 	return 1;
	// }
	// printf("Mapped address: %p\n", ptr);

	// printf("Access: first and last page...\n");
	// *(int *)ptr = 10;
	// *(int *)(ptr + (N-1)*sizeof(int)) = 10;

	// printf("calling syscall 450 - mapspages\n");
	// out_size = syscall(450, ptr, ptr + N*sizeof(int), buf, max_size);

	// printf("\n ----- Output ----- \n");
	// printf("%s\n", buf);

	// munmap ( ptr, N*sizeof(int));

    return 0;
}
// int main(int argc, char* argv[]) {
// 	size_t out_size = 0;
// 	size_t max_size = 400;
// 	char buf[400] = {0};
// 	int N=5000;
// 	void *ptr = NULL;

// 	ptr = mmap ( NULL, N*sizeof(int), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
// 	if (ptr == MAP_FAILED) {
//     	printf("Mapping Failed\n");
//     	return 1;
// 	}
// 	printf("Mapped address: %p\n", ptr);

// 	printf("Access: first and last page...\n");
// 	*(int *)ptr = 10;
// 	*(int *)(ptr + (N-1)*sizeof(int)) = 10;

// 	printf("calling syscall 450 - mapspages\n");
// 	out_size = syscall(450, ptr, ptr + N*sizeof(int), buf, max_size);

// 	printf("\n ----- Output ----- \n");
// 	printf("%s\n", buf);
//     return 0;
// }