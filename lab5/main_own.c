#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>

//Define number of readers, writers, and the number of reads and writes before the program terminates
#define NUM_READERS 3
#define NUM_WRITERS 3
#define MAX_READS 25
#define MAX_WRITES 10

//Declare variable for file
FILE *file;

//Int counter for the number of readers reading
int read_count = 0;

//Using atomic flag to mimic binary semaphores instead of using the library
atomic_flag rw_mutex = ATOMIC_FLAG_INIT;
atomic_flag mutex = ATOMIC_FLAG_INIT;

void acquire_rw_mutex() {
    //Tries to acquire the binary semaphore, and continues to do so until it succeeds
    while(atomic_flag_test_and_set(&rw_mutex));
}

void release_rw_mutex() {
    //Clears the flag, which is basically the same as releasing the lock
    atomic_flag_clear(&rw_mutex);
}

void acquire_mutex() {
    //Same thing but for the other binary semaphore
    while(atomic_flag_test_and_set(&mutex));
}

void release_mutex() {
    //Same thing but for the other binary semaphore
    atomic_flag_clear(&mutex);
}

//Function for reader
void *reader(void *arg) {
	//Variable to hold read last line
	char lastLine[256];

	//Loop, MAX_READS times
	for (int i = 0; i < MAX_READS; i++) {
		//Acquire mutex so other readers can't access the next section
		acquire_mutex();
		//Increment read_counts
		read_count++;
		//If read counts = 1, there were no readers and writers were able to write
		if (read_count == 1) {
			//Acquire rw_mutex so writers can't write
			acquire_rw_mutex();
		}
		release_mutex();

		//Get the last line of the file, start by moving pointer to the beginning
		fseek(file, 0, SEEK_SET);
		//Then read through file until next is NULL
		while (fgets(lastLine, sizeof(lastLine), file)) {
			//Reads last line
		}

		//Print read last line and the reader to terminal
		printf("Reader %d: Last line: %s\n", *((int *)arg), lastLine);

		//Acquire mutex from the start again
		acquire_mutex();
		//Decrement read_count
		read_count--;
		//If read count is now 0, there are no more readers and writers can now write
		if (read_count == 0) {
			//Release rw_ mutex so writers can write
			release_rw_mutex();
		}

		//Release mutex
		release_mutex();
	}

	//Exit thread
	pthread_exit(NULL);
}

//Function for writer
void *writer(void *arg) {
	//Variable for the data to input
	char data[256];

	//Loop, MAX_WRITES times
	for (int i = 0; i < MAX_WRITES; i++) {
		//Acquire mutex so readers can't read and other writers cannot write
		acquire_mutex();

		//Assign correct string to data
		sprintf(data, "Data %d written by Writer %d\n", i, *((int *)arg));

		//Put data to file
		fputs(data, file);
		fflush(file);

		//Print to terminal
		printf("Writer %d: Wrote data\n", *((int *)arg)); 

		//Release mutex so readers can write or other writers can write
		release_mutex();
	}

	//Exit thread
	pthread_exit(NULL);
}

//Main function
int main() {
	//Define two arrays, readers and writers, for their respective threads
	pthread_t readers[NUM_READERS], writers[NUM_WRITERS];
	//i is just for the for loops, and an id array with the combined size of readers and writers
	int i, id[NUM_READERS + NUM_WRITERS];

	//Open input.txt
	file = fopen("input.txt", "a+");

	//Create reader threads
	for (i = 0; i < NUM_READERS; i++) {
		id[i] = i + 1;
		pthread_create(&readers[i], NULL, reader, &id[i]);
	}

	//Create writer threads
	for (i = 0; i < NUM_WRITERS; i++) {
		id[NUM_READERS + i] =  i + 1;
		pthread_create(&writers[i], NULL, writer, &id[NUM_READERS + i]);
	}

	//Join reader threads
	for (i = 0; i < NUM_READERS; i++) {
		pthread_join(readers[i], NULL);
	}

	//Join writer threads
	for (i = 0; i < NUM_WRITERS; i++) {
		pthread_join(writers[i], NULL);
	}

	//Close input.txt
	fclose(file);

	return 0;
}
