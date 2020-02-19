/* Amanda Lawrence
*  March 11, 2019
*  CS 344: Operating Systems
*  Project 4: One-Time Pad
*  File: keygen
*  
*  This program creates a key file of specified length. The characters in the file generated will be any of the 27 allowed characters (alphabet letters and ' '),
*  generated using the standard UNIX randomization methods. Do not create spaces every five characters, as has been historically done. 
*  Note that you specifically do not have to do any fancy random number generation: we're not looking for cryptographically secure random number generation. 
*  The last character keygen outputs should be a newline. 
*  All error text must be output to stderr, if any.
*  
*/



#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>


#define MAXCHARS 100000
char outputArr[MAXCHARS] = "";
typedef enum {false = 0, true = 1} bool;                /* defines boolean values, true and false. */


void generateKeys(int, char *, int);




/*********************************************************************************************************************
*  Main Function:
*  Main will take user input, and the syntax is as follows
*		keygen keylength
*			- where keylength is the length of the key file in characters. keygen outputs to stdout. 
*			- We should also be able to redirect the stdout of keygen keylength to a key file like: keygen 256 > mykey
*  
*********************************************************************************************************************/

int main (int argc, char *argv[]) {


	char outputFileName[200] = "";
	int result_out;
	//int numOfChars = *argv[1] - '0';
	int numOfChars = atoi(argv[1]);


	// make sure the user has entered 2 or more arguments on command line
	if (argc == 1) {
		printf("Please enter desired number of characters to include in key.\n");
		fflush(stdout);
		exit(1);
	}

	// If the command line input does not redirect to a file, we print the content out to stdout
	// If the command line input does include a redirect to a file, we send stdout to that file, creating that file in the process. 
	if ( argc == 2 ) {
		generateKeys(numOfChars, outputArr, MAXCHARS);
		printf("%s", outputArr);
		fflush(stdout);
	} else if ( (strcmp((const char *)(size_t)argc, (const char *)(size_t)4)) && (strcmp(argv[2], ">")) ) {
		strcpy(outputFileName, argv[3]);

		int targetFD = open(outputFileName, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (targetFD == -1) { 
			fprintf(stderr, "cannot open file for output\n"); 
			fflush(stdout);
			exit(1); 
		}
		result_out = dup2(targetFD, 1);
		if (result_out == -1) { 
			fprintf(stderr, "target dup2() failed\n"); 
			fflush(stdout);
			exit(2); 
		}
		// Close on exec, the targetFD file descriptor.
		fcntl(targetFD, F_SETFD, FD_CLOEXEC);

	}

}


// generates text to be returned to either stdout or out file
void generateKeys(int amount, char *result, int maxchars){
	char tempLetter;
	char space[] = " ";
	fflush(stdout);
	int temp = 0;
	srand(time(NULL));					/* seed random with time, otherwise it will generate same numbers, in same order everytime program is run */
	int i;
	int k = 0;
	if ( amount > maxchars ) {
		fprintf(stderr, "Output string too short!\n");
		abort();
	}

	for (i=0; i < amount; i++){
		fflush(stdout);
		temp = (int)((double)rand() * (double)27 / (double)RAND_MAX);
		temp += 64;			// We want to get an integer from 64 to 90.
		tempLetter = (char) temp;
		if (tempLetter == '@') tempLetter = ' ';
		result[k++] = tempLetter;
		fflush(stdout);
	}
	result[k] = '\n';
	return;

}








