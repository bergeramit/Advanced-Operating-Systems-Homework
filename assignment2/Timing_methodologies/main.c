#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

#define SECONDS_TO_NANOSECONDS (1000000000)

long long get_processor_speed_in_nanoseconds(char *cpuinfo_line) {
	long long processor_speed_ext = 0;
	double processor_speed_base = 0;
	long long processor_speed_seconds = 0;
	long long processor_speed = 0;
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
	printf("processor_speed_base: %f, processor_speed_ext: %lld\n", processor_speed_base, processor_speed_ext);
	processor_speed_seconds = processor_speed_base * processor_speed_ext;
	processor_speed = processor_speed_seconds / SECONDS_TO_NANOSECONDS;

Exit:
	return processor_speed;
}

long long gethosttime(long long cycles) {
	FILE *fp = fopen("/proc/cpuinfo", "r");
	size_t n = 0;
	long long processor_speed = 0;
	long long cycles_in_nanoseconds = 0;
	char *line = NULL;

	while (getline(&line, &n, fp) > 0) {
			if (strstr(line, "model name")) {
					printf("%s", line);
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

int main() {
	long long cycles_in_nanoseconds = 0;
	long long cycles = getcycles();

	printf("cycles: %lld\n", cycles);
    cycles_in_nanoseconds = gethosttime(cycles);
	printf("cycles in nanoseconds: %lld\n", cycles_in_nanoseconds);
}
