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
		//test1, V
		//test2, V
		//test3, V
		//test4, V
		//test5, V
		//test6, V
		//test7, ?
		test8,
		//test9 V
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


    return 0;
}
