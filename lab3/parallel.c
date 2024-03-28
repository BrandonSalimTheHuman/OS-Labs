#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>

//Number of threads
#define NUM_THREADS 4

//There are 26 letters
#define NUM_LETTERS 26

//Array to store results, with NUM_LETTERS for size, all starting at 0
int letter_counts[NUM_LETTERS] = {0};

//Declare variable for input file
FILE *input_file;

//Create a struct. This has two attributes, the thread id and the file size
typedef struct {
	int thread_id;
	int file_size;
} ThreadArgs;

//Function for each thread
void *count_letters(void *arg) {
	//Turn arg  into ThreadArgs type
	ThreadArgs *args = (ThreadArgs *)arg;

	//Get thread id and file size
	int thread_id = args -> thread_id;
	int file_size = args -> file_size;

	//Variable c to hold the character being read
	int c;

	//Define start and end for the thread, which defines the range
	int start = thread_id * (file_size / NUM_THREADS);
	int end = start + (file_size / NUM_THREADS);

	//Send the pointer within the file to the beginning
	fseek(input_file, start, SEEK_SET);

	//Loop until end of range / next character is the end of the file
	while (ftell(input_file) < end && (c = fgetc(input_file)) != EOF) {
		//Check if it's an alphabet
		if (isalpha(c)) {
			//Check if uppercase letter
			if (isupper(c)) {
				//Subtract ASCII value of 'A' from c
				letter_counts[c - 'A']++;
			}

			else { //lowercase letter
				//Subtract ASCII value of 'a' from c
				letter_counts[c - 'a']++;
			}
		}
	}

	//Exit thread
	pthread_exit(NULL);
}


//Main function
int main(int argc, char *argv[]) {
	//Open input file in reading mode
	input_file = fopen(argv[1], "r");

	//This section is to get the file size

	//Set pointer to end of the file
	fseek(input_file, 0, SEEK_END);

	//Get pointer position relative to the beginning of the file
	long file_size = ftell(input_file);

	//Reset pointer to the beginning of th efile
	rewind(input_file);

	//Initialize array for the threads and their ids
	pthread_t threads[NUM_THREADS];
	ThreadArgs thread_args[NUM_THREADS];

	//Create threads according to NUM_THREADS
	for (int i = 0; i < NUM_THREADS; i++) {
		thread_args[i].thread_id = i;
		thread_args[i].file_size = file_size;
		pthread_create(&threads[i], NULL, count_letters, &thread_args[i]);
		pthread_join(threads[i], NULL);
	}

	//Close input file
	fclose(input_file);

	//Open output file in writing mode
	FILE *output_file = fopen("output.txt", "w");

	//Loop through the array and write it to the file
	for (int i = 0; i < NUM_LETTERS; i++) {
		fprintf(output_file, "%c  %d\n", 'A' + i, letter_counts[i]);
	}

	//Close output file
	fclose(output_file);

	return 0;
}
