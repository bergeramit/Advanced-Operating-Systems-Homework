#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <string.h>
#include <signal.h>
#include "mapspages_tests.h"

#define MAX_BUF_SIZE                2400000
#define MAX_CHILDREN                 1000
#define GET_NUM_PAGES(num_entries)  ((((num_entries) * sizeof(int)) / (double)getpagesize()) + 1)
#define GET_NUM_ENTRIES(num_pages)  ((int)(((getpagesize() * (num_pages)) / sizeof(int)) - 1))
#define SLEEP_TIMEOUT               1
#define TO_INT(c)                   ((c) - '0')
#define START_STACK_INDEX           27

#ifndef DEBUG_EN
#define DEBUG_EN    1
#endif

static int pages_write_indexes[MAX_BUF_SIZE];
static pid_t children_to_kill[MAX_CHILDREN];

int print_maps(void *start, void *end)
{
    size_t out_size = 0;
	size_t max_size = 400;
	char buf[MAX_BUF_SIZE] = {0};

    out_size = syscall(450, start, end, buf, max_size);
    printf("\n ----- Output ----- \n");
    printf("out_size = %ld\n", out_size);
    printf("%s\n", buf);

    return 0;
}


static int tests_helper(int indexes_len, bool should_unmap) {
    size_t out_size = 0;
	size_t max_size = MAX_BUF_SIZE;
	char buf[MAX_BUF_SIZE] = {0};
	int N = GET_NUM_ENTRIES(indexes_len);
	void *ptr = NULL;

#if DEBUG_EN
    printf("(DEBUG)[%s/%d]: number of pages: %d\n", 
        __func__,
        __LINE__,
        indexes_len);
#endif

	ptr = mmap ( NULL, N*sizeof(int), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	if (ptr == MAP_FAILED) {
    	printf("Mapping Failed\n");
    	return 1;
	}
	printf("Mapped address: %p\n", ptr);

    for (int i = 0; i < indexes_len; i++) {
        if (pages_write_indexes[i]) {
            *(int *)(ptr + (i * getpagesize())) = 10;
        }
    }

    print_maps(ptr, ptr + N*sizeof(int));

    if (should_unmap) {
        munmap ( ptr, N*sizeof(int));
    }

    int count = 0, i = 0;
    while (buf[i] && buf[i] != '.') i++;
    while (buf[i] && buf[i] == '.') {count++; i++;}
#if DEBUG_EN
    printf("(DEBUG)[%s/%d]: count: %d\n", 
        __func__,
        __LINE__,
        count);
#endif
	return 0;
}

int test1(void) {
	int N = GET_NUM_ENTRIES(10);
    int num_of_pages = GET_NUM_PAGES(N);

    printf("Expecting: '..........'\n");
    printf("num of pages: %d\n", num_of_pages);
    for (int i = 0; i < num_of_pages; i++) {
        pages_write_indexes[i] = 0;
    }
    return tests_helper(num_of_pages, true);
}

int test2(void) {
    int N = GET_NUM_ENTRIES(10);
    int num_of_pages = GET_NUM_PAGES(N);

    printf("Expecting: '1111111111'\n");
    printf("num of pages: %d\n", num_of_pages);

    for (int i = 0; i < num_of_pages; i++) {
        pages_write_indexes[i] = 1;
    }

    return tests_helper(num_of_pages, true);
}

int test3(void) {
    int N = GET_NUM_ENTRIES(10);
    int num_of_pages = GET_NUM_PAGES(N);

    printf("Expecting: '.1.1.1.1.1'\n");
    printf("num of pages: %d\n", num_of_pages);

    for (int i = 0; i < num_of_pages; i++) {
        if (i % 2) {
            pages_write_indexes[i] = 1;
        }
        else {
            pages_write_indexes[i] = 0;
        }
    }

    return tests_helper(num_of_pages, true);
}

int test4(void) {
    size_t out_size = 0;
	size_t max_size = 400;
	char buf[MAX_BUF_SIZE] = {0};
	int N = GET_NUM_ENTRIES(10);
	void *ptr = NULL;
    int num_of_pages = GET_NUM_PAGES(N);
    pid_t child_pid;

    printf("Expecting: '22222.....'\n");
    printf("num of pages: %d\n", num_of_pages);

    ptr = mmap ( NULL, N*sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
	if (ptr == MAP_FAILED) {
    	printf("Mapping Failed\n");
    	return 1;
	}
	printf("Mapped address: %p\n", ptr);

	printf("calling syscall 450 - mapspages\n");
    child_pid = fork();

    if (child_pid == 0) {
        for (int i = 0; i < num_of_pages; i++) {
            if (i < 5) {
                *(int *)(ptr + (i * getpagesize())) = 10;
            }
        }
	    // out_size = syscall(450, ptr, ptr + N*sizeof(int), buf, max_size);
        while(1){};
    }
    else {
        for (int i = 0; i < num_of_pages; i++) {
            if (i < 5) {
                *(int *)(ptr + (i * getpagesize())) = 10;
            }
        }

        sleep(SLEEP_TIMEOUT);
        out_size = syscall(450, ptr, ptr + N*sizeof(int), buf, max_size);

        printf("\n ----- Output ----- \n");
        printf("%s\n", buf);
        
        kill(child_pid, SIGKILL);
        wait(NULL);

        munmap (ptr, N*sizeof(int));
    }

	return 0;
}

int test5(void) {
    size_t out_size = 0;
	size_t max_size = 400;
	char buf[MAX_BUF_SIZE] = {0};
	int N = GET_NUM_ENTRIES(10);
	void *ptr = NULL;
    int num_of_pages = GET_NUM_PAGES(N);
    pid_t child_pid;

    printf("Expecting: '1111..2222'\n");
    printf("num of pages: %d\n", num_of_pages);

    ptr = mmap ( NULL, N*sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
	if (ptr == MAP_FAILED) {
    	printf("Mapping Failed\n");
    	return 1;
	}
	printf("Mapped address: %p\n", ptr);

	printf("calling syscall 450 - mapspages\n");
    child_pid = fork();

    if (child_pid == 0) {
        for (int i = 0; i < num_of_pages; i++) {
            if (i >= 6) {
                *(int *)(ptr + (i * getpagesize())) = 10;
            }
        }
	    // out_size = syscall(450, ptr, ptr + N*sizeof(int), buf, max_size);
        while(1){};
    }
    else {
        for (int i = 0; i < num_of_pages; i++) {
            if (i != 4 && i != 5) {
                *(int *)(ptr + (i * getpagesize())) = 10;
            }
        }

        sleep(SLEEP_TIMEOUT);

        out_size = syscall(450, ptr, ptr + N*sizeof(int), buf, max_size);

        printf("\n ----- Output ----- \n");
        printf("%s\n", buf);
        
        kill(child_pid, SIGKILL);
        wait(NULL);

        munmap (ptr, N*sizeof(int));
    }

	return 0;
}

int test6(void) {
    int N = GET_NUM_ENTRIES(2000);
    int num_of_pages = GET_NUM_PAGES(N);

    printf("Expecting: 200*'.'\n");
    printf("num of pages: %d\n", num_of_pages);

    for (int i = 0; i < num_of_pages; i++) {
        pages_write_indexes[i] = false;
    }

    return tests_helper(num_of_pages, true);
}

static inline int hit_page(void *target_page_addr, int amount, int open_processes)
{
    int i = 0;
    pid_t child_pid;

    for (int i = 0; i < amount; i++) {
        child_pid = fork();
        *(int *)(target_page_addr) = 10;
        if (child_pid == 0) {
            while(1){};
        } else {
            children_to_kill[open_processes + i] = child_pid;
        }
    }
    return i;
}

uint64_t getsp( void )
{
    uint64_t sp;
    asm( "mov %%rsp, %0" : "=rm" ( sp ));
    return sp;
}

int test7(void)
{
    FILE *fp;
    int status;
    int i = 0;
    size_t out_size = 0;
	size_t max_size = 400;
	char buf[MAX_BUF_SIZE] = {0};
    char values[500];
    void *stack_address;
    void *current_esp;
    char * token;

    fp = fopen("/proc/self/stat", "r");
    if (fp == NULL) {
        printf("Failed to popen\n");
        return 1;
    }

    fgets(values, 500, fp);
    printf("%s\n", values);

    token = strtok(values, " ");
    // loop through the string to extract all other tokens
    while( token != NULL && i != START_STACK_INDEX) {
        token = strtok(NULL, " ");
        i++;
    }
    stack_address = (void *)atol(token);
    printf( "Stack: %p\n", stack_address);
    //current_esp = (void *)__builtin_frame_address(1);
    current_esp = (void *)getsp();
    printf("Current esp: %p\n", current_esp);
    
    out_size = syscall(450, current_esp, stack_address, buf, max_size);
    printf("\n ----- Output ----- \n");
    printf("outsize = %ld\n", out_size);
    printf("%s\n", buf);

    pclose(fp);
}

int test8(void)
{
    size_t out_size = 0;
	size_t max_size = 400;
	char buf[MAX_BUF_SIZE] = {0};
    char *pattern = "1234..X";
    int N = GET_NUM_ENTRIES(strlen(pattern));
    int num_of_pages = GET_NUM_PAGES(N);
	void *ptr = NULL;
    int i = 0;
    int total_open_processes = 0;
    ptr = mmap ( NULL, N*sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);

    for (i=0; i<strlen(pattern); i++){
        switch(pattern[i]) {
            case '.':
                pages_write_indexes[i] = 0;
                break;
            case 'X':
                pages_write_indexes[i] = 11;
                break;
            default:
                pages_write_indexes[i] = TO_INT(pattern[i]);
                break;
        }
        total_open_processes += hit_page(ptr + (i * getpagesize()), pages_write_indexes[i], total_open_processes);
    }

    sleep(SLEEP_TIMEOUT);
    print_maps(ptr, ptr + N*sizeof(int));

    for (i=0; i<total_open_processes; i++) {
        kill(children_to_kill[i], SIGKILL);
    }
    printf("Children Killed\n");
    munmap(ptr, N*sizeof(int));
    return 0;
}

int test9(void)
{
    int num_of_pages = 240000;
    int res = 0;

    printf("Expecting: OOM\n");
    printf("num of pages: %d\n", num_of_pages);

    for (int i = 0; i < num_of_pages; i++) {
        pages_write_indexes[i] = true;
    }

    res = tests_helper(num_of_pages, false);
    res = tests_helper(num_of_pages, false);
    res = tests_helper(num_of_pages, false);
    res = tests_helper(num_of_pages, false);
    while(1){};
}
