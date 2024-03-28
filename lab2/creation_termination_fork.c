#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main() {
	pid_t pid = fork();
	if (pid == 0) { //Child process
		printf("Created child process with process id: %d\n", getpid());
		printf("Terminating child process with process id: %d\n", getpid());
		exit(0);
	}

	else if (pid > 0) { //parent process
		printf("Parent process with pid %d, with child process with pid %d\n", getpid(), pid);
		wait(NULL); //wait for child to terminate
		printf("Parent process detects child process has been terminated\n");
	}

	return 0;
}
