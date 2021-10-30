#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>


typedef int Pipefd[2];

void run_command(char *command) {
	char *args[3] = {command, NULL, NULL}; 
	execvp(command, args);
}


int pipe_commands(char* commands[], int number_of_commands, Pipefd *piped_fds) {
	int i = 0;
	pid_t child_pid;

	for (i = 1; i < number_of_commands; i++) {
		child_pid = fork();

		if ( child_pid != 0 ) {
			printf("Running: %s\n", commands[i]);
			wait(NULL);
			continue;
		}

		if (i == 1) {
			// First command to run should not direct stdin anywhere
			dup2(piped_fds[i-1][1], STDOUT_FILENO);
		} else if (i == number_of_commands - 1) {
			// Last command to run should not direct stdout anywhere
			dup2(piped_fds[i-2][0], STDIN_FILENO);
		} else {
			// Should handle stdout and stdin for that process
			dup2(piped_fds[i-2][0], STDIN_FILENO);
			dup2(piped_fds[i-1][1], STDOUT_FILENO);
		}
		run_command(commands[i]);

	}
	wait(NULL);
}

void create_pipes(Pipefd *piped_fds, int amount) {
	int i = 0;

	for (i = 0; i < amount; i++) {
		if (pipe(piped_fds[i]) == -1) {
		       perror("pipe");
		       return;
		}
	}
}

int main(int argc, char* argv[]) {
	Pipefd *piped_fds;

	if (argc < 3) {
		printf("usage: piper ARG1 ARG2 [...]\n");
		return 1;
	}

	piped_fds = (Pipefd *)malloc( (argc - 1) * sizeof(Pipefd));

	create_pipes(piped_fds, argc - 1);
	pipe_commands(argv, argc, piped_fds);

	printf("Done!\n");

	return 0;
}
