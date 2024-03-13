/*Write a program that executes one command (with arguments) and
	communicates its output to another process that executes another command (with arguments).
Requirements:
- Use pipe(2)
- Test your code by checking that the result of your program matches the Unix pipe operator |
For example, given the command1_args and command2_args values in the skeleton code, the output of the program should be identical to running ls -l | sort on the command line.
- (Optional) After you make sure pipe is working, modify the program to make it accepting command 1 and command 2 from the user.
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	int pipefd[2];
	int pid;

	char *command1_args[] = {"ls", "-la", NULL};
	char *command2_args[] = {"sort", NULL};

	if (pipe(pipefd) == -1) {
		perror("pipe");
		return 1;
	}

	pid = fork();
	if (pid == -1) {
		perror("fork");
		return 1;
	}

	if (pid == 0) {  // Child process - run ls
		close(pipefd[STDIN_FILENO]);  // Close unused read end
		dup2(pipefd[STDOUT_FILENO], STDOUT_FILENO);  // Redirect stdout to pipe's write end
		close(pipefd[STDOUT_FILENO]);

		if (execvp(command1_args[0], command1_args) == -1) {
			perror("execvp ls");
			return 1;
		}
		return 0;
	}

	// Parent process - run sort
	close(pipefd[1]);  // Close unused write end
	dup2(pipefd[0], STDIN_FILENO);  // Redirect stdin to pipe's read end
	close(pipefd[0]);

	if (execvp(command2_args[0], command2_args) == -1) {
		perror("execvp sort");
		return 1;
	}

	wait(NULL);  // Wait for child process to finish
	return 0;
}

