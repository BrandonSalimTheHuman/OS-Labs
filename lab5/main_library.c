#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

//Define number of readers, waiters, and a limit for each operation so they can terminate
#define NUM_READERS 3
#define NUM_WRITERS 3
#define MAX_WRITES 10
#define MAX_READS 25

//Declare variable for file (input.txt)
FILE *file;

//Declare two binary semaphores: mutex and rw_mutex
sem_t mutex, rw_mutex;

//An int variable to count the number of readers currently reading
int read_count = 0;

//Function for reader
void *reader(void *arg) {
	//Variable to hold the last line of the file being read
	char lastLine[256];

	//For loop based on the max_reads variable defined above
	for (int i = 0; i < MAX_READS; i++) {
		//Wait for the first mutex, which stops multiple readers from changing read count simultaneously
		sem_wait(&mutex);
		//Increment read count
		read_count++;
		//If read count is now 1, they was previously no readers, and writers were able to write
		if (read_count == 1){
			//Decrement rw_mutex so writers can no longer write
			sem_wait(&rw_mutex);
		}
		//Increment mutex so other readers can access this section
		sem_post(&mutex);

		//Sets pointer to the start of the file
		fseek(file, 0, SEEK_SET);

		//Reads through lines until the next one is NULL, meaning this searches the last line
		while (fgets(lastLine, sizeof(lastLine), file)) {
			//Reading the last line
		}

		//Print the line and the reader to the terminal
		printf("Reader %d: Last line: %s\n", *((int *)arg), lastLine);

		//Decrement the mutex used at the start again
		sem_wait(&mutex);
		//Decrement read_count
		read_count--;
		//If read count is now 0, then writers are now able to write
		if (read_count == 0) {
			//Increment rw_mutex so writers can write
			sem_post(&rw_mutex);
		}
		//Increment mutex so other readers can access this section
		sem_post(&mutex);
	}
	//Exits thread
	pthread_exit(NULL);
}

//Function for writer
void *writer(void *arg) {
	//Variable to hold that text that's inputted
	char data[256];

	//Loops through MAX_WRITES times
	for (int i = 0; i < MAX_WRITES; i++) {
		//Decrement rw_mutex so that readers cannot read
		sem_wait(&rw_mutex);

		//Assign the appropriate string to the data variable
		sprintf(data, "Data %d written by Writer %d\n", i, *((int *)arg));
		//Puts data in the file
		fputs(data, file);
		fflush(file);

		//Print to the terminal
		printf("Writer %d: Wrote data\n", *((int *)arg));

		//Increments rw_mutex so readers can read again / other writers can write
		sem_post(&rw_mutex);
	}

	//Exit thread
	pthread_exit(NULL);
}

//Mai function
int main() {
	//Declare two arrays, one for reader threads, one for writer threads
	pthread_t readers[NUM_READERS], writers[NUM_WRITERS];
	//i is just for the many for loops, but also an id array with the combined size of both threads
	int i, id[NUM_READERS + NUM_WRITERS];

	//Open input.txt in a+ mode (reading and appending)
	file = fopen("input.txt", "a+");

	//Initialize the two binary semaphores
	sem_init(&mutex, 0, 1);
	sem_init(&rw_mutex, 0, 1);

	//Creating reader threads
	for (i = 0; i < NUM_READERS; i++) {
		id[i] = i + 1;
		pthread_create(&readers[i], NULL, reader, &id[i]);
	}

	//Creating writer threads
	for (i = 0; i < NUM_WRITERS; i++) {
		//The id continues from the readers, so NUM_READERS + i is needed
		id[NUM_READERS + i] = i + 1;
		pthread_create(&writers[i], NULL, writer, &id[NUM_READERS + i]);
	}

	//Join the reader threads
	for (i = 0; i < NUM_READERS; i++) {
		pthread_join(readers[i], NULL);
	}

	//Join the writer threads
	for (i = 0; i < NUM_WRITERS; i++) {
		pthread_join(writers[i], NULL);
	}

	//Close input.txt
	fclose(file);

	//Destroy the two semaphores
	sem_destroy(&mutex);
	sem_destroy(&rw_mutex);

	return 0;
}
