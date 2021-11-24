#ifndef _MAPSPAGES_TESTS_
#define _MAPSPAGES_TESTS_

#include <string.h>

#define MAPSPAGES_TESTS_NUM 9
#define MAX_PATTERN_LEN     100
#define TO_INT(c)           ((c) - '0')

typedef int (*testfunc)(void);

int test1(void);
int test2(void);
int test3(void);
int test4(void);
int test5(void);
int test6(void);
int test7(void);
int test8(char *pattern);
int test9(void);

#endif