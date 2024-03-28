#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

//There are 26 letters
#define NUM_LETTERS 26

int main(int argc, char *argv[]){
	//Making sure there are two inputs
	if (argc != 2) {
		return 1;
	}

	//Open input file in reading mode
	FILE *input_file = fopen(argv[1], "r");

	//Array to store results, with NUM_LETTERS for size, all starting at 0
	int letter_counts[NUM_LETTERS] = {0};

	//Variable c to hold the character being read
	int c;

	//While the next character isn't the end of the file
	while ((c = fgetc(input_file)) != EOF) {
		//Check if it's an alphabet
		if (isalpha(c)) {
			//Check if uppercase letter
			if (isupper(c)) {
				//Subtract ASCII value of "A" from c
				letter_counts[c - 'A']++;
			}

			else { //lowercase letter
				//Subtract ASCII value of "a" from c
				letter_counts[c - 'a']++;
			}
		}
	}

	//Close input file
	fclose(input_file);

	//Open output file, in writing mode
	FILE *output_file = fopen("output.txt", "w");

	//Loop through the array and write it to the file
	for (int i = 0; i < NUM_LETTERS; i++) {
		fprintf(output_file, "%c  %d\n", 'A' + i, letter_counts[i]);
	}

	//Close output file
	fclose(output_file);

	return 0;
}
