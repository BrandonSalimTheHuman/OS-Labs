#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <sys/wait.h>

//Main function
int main () {
	//Generate key for the shared memory
	key_t key = ftok("/tmp", 'A');

	//Create shared memory segment
	int shmid = shmget(key, 1024, 0666 | IPC_CREAT);

	//Attach shared memory
	char *shmaddr = (char *)shmat(shmid, NULL, 0);

	//Writing data
	strcpy(shmaddr, "This is shared memory");

	//Detatch the shared memory
	shmdt(shmaddr);

	//Fork for child process
	pid_t pid = fork();

	if (pid == 0) { //For child process
		//Attach shared memory
		char *child_shmaddr = (char *)shmat(shmid, NULL , 0);

		//Reading data
		printf("Shared memory read by child process: %s\n", child_shmaddr);
		
		//Detach from shared memory
		shmdt(child_shmaddr);
        }
        
        else if (pid > 0) { //For parent process
                //Wait for child process to terminate
                wait(NULL);
                
                //Remove shared memory
                shmctl(shmid, IPC_RMID, NULL);
        }
        
        return 0;
}
