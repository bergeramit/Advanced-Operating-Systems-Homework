#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>

#define SECONDS_TO_NANOSECONDS (1e+9)
#define SAMPLE_SIZE (1000)

long double get_processor_speed_in_nanoseconds(char *cpuinfo_line) {
	long double processor_speed_ext = 0;
	double processor_speed_base = 0;
	long double processor_speed_seconds = 0;
	long double processor_speed = 0;
	char *base_pointer = NULL;
	
	base_pointer = strstr(cpuinfo_line, " @ ");
	if (base_pointer == NULL) {
		processor_speed = 0;
		goto Exit;
	}
	processor_speed_base = atof(base_pointer + 3);

	if (strstr(cpuinfo_line, "GHz")) {
			processor_speed_ext = 1024*1024*1024;
	} else if (strstr(cpuinfo_line, "MHz")) {
			processor_speed_ext = 1024*1024;
	} else if (strstr(cpuinfo_line, "KHz")) {
			processor_speed_ext = 1024;
	} else if (strstr(cpuinfo_line, "Hz")) {
			processor_speed_ext = 1;
	}
	/* printf("processor_speed_base: %f, processor_speed_ext: %Lf\n", processor_speed_base, processor_speed_ext); */
	processor_speed_seconds = processor_speed_base * processor_speed_ext;
	processor_speed = processor_speed_seconds / SECONDS_TO_NANOSECONDS;

Exit:
	return processor_speed;
}

long double gethosttime(long double cycles) {
	FILE *fp = fopen("/proc/cpuinfo", "r");
	size_t n = 0;
	long double processor_speed = 0;
	long double cycles_in_nanoseconds = 0;
	char *line = NULL;

	while (getline(&line, &n, fp) > 0) {
			if (strstr(line, "model name")) {
					/* printf("%s", line); */
					processor_speed = get_processor_speed_in_nanoseconds(line);
					break;
			}
	}
	if (!processor_speed) {
		printf("Error in get_processor_speed_in_nanoseconds. Exiting...\n");
		goto Exit;
	}
	cycles_in_nanoseconds = cycles / processor_speed;

Exit:
	free(line);
	fclose(fp);
	return cycles_in_nanoseconds;
}

static inline unsigned long long getcycles(void)
{
	unsigned long low, high;
	asm volatile ("rdtsc" : "=a" (low), "=d" (high));
	return ((low) | (high) << 32);
}

long double calculate_gettimeofday_diff(struct timeval *start_tv, struct timeval *end_tv)
{
	return ((end_tv->tv_sec - start_tv->tv_sec) + ((end_tv->tv_usec - start_tv->tv_usec) * 1e-6)) * SECONDS_TO_NANOSECONDS;
}

void calculate_standard_deviation(long double data[SAMPLE_SIZE], long double *out_std, long double *out_mean) {
    long double sum = 0.0, standard_diviation = 0.0;
    int i;

    for (i = 0; i < SAMPLE_SIZE; ++i) {
        sum += data[i];
    }
    *out_mean = sum / SAMPLE_SIZE;
    for (i = 0; i < SAMPLE_SIZE; ++i) {
        standard_diviation += powl(data[i] - *out_mean, 2);
    }
    *out_std = sqrtl(standard_diviation / SAMPLE_SIZE);
}

void time_inner_loop(void)
{
	int i = 0, j = 0, k = 0;
	struct timeval start_tv = {0};
	struct timeval end_tv = {0};
	long double start = 0;
	long double measurements[SAMPLE_SIZE] = {0};
	long double std, mean;

	printf("\n(c) Calculate std and mean for gethosttime and gettimeofday\n");
	for (i=0; i < SAMPLE_SIZE; i++) {
		start = getcycles();
        for (j=0; j < 100; j++) {  /* inner loop starts here */
            k = i + j;  
        }                          /* inner loop ends here */
		measurements[i] = gethosttime(getcycles() - start);
    }
	(void)calculate_standard_deviation(measurements, &std, &mean);
	printf("(c.1) for gethosttime: std: %Lf mean: %Lf\n", std, mean);

	for (i=0; i < SAMPLE_SIZE; i++) {
		gettimeofday(&start_tv, NULL);
        for (j=0; j < 100; j++) {  /* inner loop starts here */
            k = i + j;  
        }                          /* inner loop ends here */
		gettimeofday(&end_tv, NULL);
		measurements[i] = calculate_gettimeofday_diff(&start_tv, &end_tv);
    }
	(void)calculate_standard_deviation(measurements, &std, &mean);
	printf("(c.2) for gettimeofday: std: %Lf mean: %Lf\n", std, mean);
}

void time_gettimeofday(void)
{
	struct timeval start_tv = {0};
	struct timeval end_tv = {0};
	long double time_in_nanoseconds = 0;
	printf("\nStart measuring gettimeofday() function\n");
	gettimeofday(&start_tv, NULL);
	gettimeofday(&end_tv, NULL);
	time_in_nanoseconds = calculate_gettimeofday_diff(&start_tv, &end_tv);
	printf("(b.2) gettimeofday() takes %Lf nanoseconds\n", time_in_nanoseconds);
}

void time_getcycles(void)
{
	long double start = 0, cycles_diff = 0;
	printf("\nStart measuring getcycles() function\n");
    start = getcycles();
	cycles_diff = getcycles() - start;
	printf("(b.1) getcycles() takes %Lf nanoseconds\n", gethosttime(cycles_diff));
}

void print_cycles_in_nanoseconds(void)
{
	long double cycles_in_nanoseconds = 0; 
	long double cycles = getcycles();

	printf("cycles: %Lf\n", cycles);
    cycles_in_nanoseconds = gethosttime(cycles);
	printf("(a) cycles in nanoseconds: %Lf\n", cycles_in_nanoseconds);
}

int main() {
	print_cycles_in_nanoseconds();
	time_getcycles();
	time_gettimeofday();
	time_inner_loop();
}
