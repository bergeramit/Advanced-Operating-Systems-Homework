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

void create_pipes(Pipefd *piped_fds, int amount) {
	int i = 0;
	for (i = 0; i < amount; i++) {
		if (pipe(piped_fds[i]) == -1) {
		       perror("pipe");
		       return;
		}
	}
}

void close_all_pipes(Pipefd *piped_fds, int amount) {
	int i = 0;
	for (i = 0; i < amount; i++) {
		close(piped_fds[i][0]);
		close(piped_fds[i][1]);
	}
}

int pipe_commands(char* commands[], int number_of_commands, Pipefd *piped_fds) {
	int i = 0, j = 0;
	pid_t child_pid;

	for (i = 0; i < number_of_commands; i++) {
		child_pid = fork();

		if (child_pid != 0) {
			continue;
		}

		if (i == 0) {
			// First command to run should not direct stdin anywhere
			dup2(piped_fds[i][1], STDOUT_FILENO);
			close(piped_fds[i][0]);
			for (j = 1; j < number_of_commands - 1; j++) {
				close(piped_fds[j][0]);
				close(piped_fds[j][1]);
			}

		} else if (i == number_of_commands - 1) {
			// Last command to run should not direct stdout anywhere
			dup2(piped_fds[i-1][0], STDIN_FILENO);
			close(piped_fds[i-1][1]);
			for (j = 0; j < i-1; j++) {
				close(piped_fds[j][0]);
				close(piped_fds[j][1]);
			}
		} else {
			// Should handle stdout and stdin for that process
			dup2(piped_fds[i-1][0], STDIN_FILENO);
			dup2(piped_fds[i][1], STDOUT_FILENO);
			close(piped_fds[i-1][1]);
			close(piped_fds[i][0]);

			for (j = 0; j < number_of_commands - 1; j++) {
				if (j != i && j != i-1) {
					close(piped_fds[j][1]);
					close(piped_fds[j][0]);
				}
			}
		}
		run_command(commands[i + 1]);
	}
}

void wait_for_all_children(int number_of_commands) {
	int i=0;
	for (i=0; i<number_of_commands; i++) {
		wait(NULL);
	}
}

int main(int argc, char* argv[]) {
	Pipefd *piped_fds;
	int number_of_commands = argc - 1;

	if (argc < 3) {
		printf("usage: piper ARG1 ARG2 [...]\n");
		return 1;
	}

	piped_fds = (Pipefd *)malloc( (argc - 2) * sizeof(Pipefd));

	create_pipes(piped_fds, number_of_commands - 1);
	pipe_commands(argv, number_of_commands, piped_fds);
	close_all_pipes(piped_fds, number_of_commands - 1);
	wait_for_all_children(number_of_commands);

	printf("Done!\n");

	return 0;
}
