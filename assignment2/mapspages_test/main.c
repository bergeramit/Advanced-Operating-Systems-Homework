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

	int test = 1;
	int ret;
	char pattern[MAX_PATTERN_LEN] = {0};
	char default_pattern[MAX_PATTERN_LEN] = {'1', '2', '3', '4', '1', '3', '.', '.', 'x'};

	if (argc != 2 || strlen(argv[1]) > 1) {
		printf("No test to run...\nTo run all tests: make test\nTo run a single test: ./test [1-9]\n");
		return 1;
	}

	test = TO_INT(argv[1][0]);
	
	if (test < 1 || test > 9) {
		printf("Invalid test num...\nTo run all tests: make test\nTo run a single test: ./test [1-9]\n");
		return 1;
	}

	if (test == 7) {
		printf("Test 7 isn't implemented, skipping...\n");
		return 0;
	}

	if (test == 9) {
		/* Adjusted for index */
		test = 7;
	}

	testfunc tests[MAPSPAGES_TESTS_NUM] = {
		test1,
		test2,
		test3,
		test4,
		test5,
		test6,
		test9
		};

	if (test == 8) {
		printf("Insert a pattern for test 8 or enter 'd' for default (123413..x): ");
		scanf("%s", pattern);
		if (pattern[0] == 'd') {
			ret = test8(default_pattern);
		}
		else {
			ret = test8(pattern);
		}
	}
	else {
		ret = tests[test - 1]();
	}
	printf("Test %d finished with retval: %d (%s)\n",
	test,
	ret,
	ret == 0 ? "SUCCESS" : "FAILED");

    return ret;
}
