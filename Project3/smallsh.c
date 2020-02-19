/* Amanda Lawrence
*  February 17, 2019
*  CS 344: Operating Systems
*  Project 3: smallsh
*  
*  Write a small shell program
*  
*  3 commands that I must write out myself: exit, cd, status.
*  All other commands can be parsed and passed into exec.
*  
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


#define NBKG 1000
#define UNUSED -1

typedef enum {false = 0, true = 1} bool;                /* defines boolean values, true and false. */

int foreground_mode = 0;	//If 0, we are allowed to set background processes. If set to 1, we are ONLY allowed to use foreground processes, no background processes.

void catchSIGTSTP(int);




/*********************************************************************************************************************
*  Main Function:
*  Main will take user input, parse each string entered, and checks for the commands: exit, cd, or status. 
*  If the user has entered any other command other than exit, cd, or status, then it will further parse the user's 
*  input to sort out and define any file names the user entered, based on redirection symbols > or <, search for an & 
*  to determine the user's preference for running command as a background process, and execute the the command, 
*  with any flags and filenames. 
*  
*********************************************************************************************************************/


int main() {

	char buf[100]; 
	char *buf_tokens;
	//pid_t pid = getpid();
	int lastExitStatus = 0;		// Exit status of most recently executed process
	int pid = getpid();			// In case the user enters "$$"


	// Variables as defined in class reading 3.3 Advanced User Input with getline()
	int numCharsEntered = -5; 	// How many chars we entered
	int currChar = -5; 			// Tracks where we are when we print out every char
	size_t bufferSize = 0; 		// Holds how large the allocated buffer is


	// Once we parse the input from the user, we'll determine if there's an input file, output file and store 
		// their names in these variables. 
	char inputFileName[200] = "";
	char outputFileName[200] = "";
	int isBackgroundProcess = 0;	// If user enters in an ampersand last, then this will be set to 1. Otherwise, it's set to 0.
	int nextIsInput = 0;
	int nextIsOutput = 0;


	// The following variables are for executing our non-built-in commands. Adapted from class lecture 3.4: More UNIX I/O
	pid_t forkPID;
	int background_pid = 0;
	int result_in;
	int result_out;
	int childExitStatus = 0;
	int exit_status = 0;
	int fileDescriptor;
	char readBuffer[8];
	memset(readBuffer, '\0', sizeof(readBuffer));

	int bkg_list[NBKG]; // space for 1000 background processes.
	for(int n = 0; n < NBKG; n++) {
		bkg_list[n] = UNUSED;
	}


	// From class reading 3.3 Advanced User Input with getline()
	// redirect ^Z to catchSIGTSTP. We will ignore an ampersand if given, and continue running all processes in the foreground until user presses ^Z again. 
	struct sigaction SIGTSTP_action = {0};
	SIGTSTP_action.sa_handler = catchSIGTSTP;
	sigfillset(&SIGTSTP_action.sa_mask);
	SIGTSTP_action.sa_flags = 0;
	if ( sigaction(SIGTSTP, &SIGTSTP_action, NULL) == -1 )
		perror("Could not install SIGTSTP handler");

	// Ignore ^C
	struct sigaction SIGINT_action = {0};
	SIGINT_action.sa_handler = SIG_IGN;
	sigfillset(&SIGINT_action.sa_mask);
	SIGINT_action.sa_flags = 0;
	sigaction(SIGINT, &SIGINT_action, NULL);



	fflush(stdout);

	while(true) {
		// Clears the lineEntered buffer of any previously entered strings.
		char* lineEntered = NULL; 	// Points to a buffer allocated by getline() that holds our entered string + \n + \0
		strcpy(inputFileName, "");
		strcpy(outputFileName, "");
		isBackgroundProcess = 0;

		// Clear the token_arr of any previously entered tokenized strings.
		char token_arr[10][100];	// Holds an array of each word/string a user has entered, individually, after it's been tokenized.
		for (int x = 0; x < 10; x++){
			for (int y = 0; y < 100; y++){
				token_arr[x][y] = '\0';
			}
		}
		char *args[11] = {NULL};
		for(int z = 0; z < 11; z++){
			args[z] = token_arr[z];
		}
		int counter = 0;


		/* check for and announce ended background processes */
		for (int i = 0; i < NBKG; i++){
			if (bkg_list[i] != UNUSED ) {
				/* check if it died! */
				int bkg_status = 0;
				pid_t result = waitpid(bkg_list[i], &bkg_status, WNOHANG);
				if ( result == bkg_list[i] ) {
					/* child exited */
					if(WIFEXITED(childExitStatus) != 0){						// If process terminated normally, then WIFEXITED macro returns non-zero. 
						exit_status = WEXITSTATUS(childExitStatus);				// Get the actual exit status with the WEXITSTATUS macro.
					} else if (WIFSIGNALED(childExitStatus) != 0) {				// If process was terminated by a signal, then WIFSIGNALED macro returns non-zero.
						exit_status = WTERMSIG(childExitStatus);				// Get terminating signal with WTERMSIG macro. 
					}

					printf("background pid %d is done: exit value %d\n", bkg_list[i], exit_status);
					fflush(stdout);
					bkg_list[i] = UNUSED;
				}
			}
		}

		// Get input from the user
		while(1) {
			printf(": ");
			fflush(stdout);
			// Get a line from the user
			numCharsEntered = getline(&lineEntered, &bufferSize, stdin);
			if (numCharsEntered == -1)
				clearerr(stdin);
			else
				break; // Exit the loop - we've got input
		}


		//The following lines split our input buffer into tokenized command, arguments, directory, filename, etc
		buf_tokens = strtok (lineEntered, " \n\t\r");
		while (buf_tokens != NULL){
			char *p;
			p = strstr(buf_tokens, "$$");

			if(strncmp(buf_tokens, "$$", 2) == 0) {
				sprintf(token_arr[counter], "%d", pid);
				counter++;
			} else if (p) {
				strcpy(token_arr[counter], buf_tokens);
				p = strstr(token_arr[counter], "$$");
				sprintf(p, "%d", pid);
				printf("We just wrote:  %s\n", token_arr[counter]);
				fflush(stdout);
				counter++;
			} else if (nextIsInput == 1) {
				strcpy(inputFileName, buf_tokens);
				nextIsInput = 0;
			} else if (nextIsOutput == 1) {
				strcpy(outputFileName, buf_tokens);
				nextIsOutput = 0;
			} else if (strncmp(buf_tokens, "<", 1) == 0) {
				nextIsInput = 1;
			} else if (strncmp(buf_tokens, ">", 1) == 0) {
				nextIsOutput = 1;
			} else if (strncmp(buf_tokens, "&", 1) == 0) {
				isBackgroundProcess = 1;
				fflush(stdout);
			} else {
				strcpy(token_arr[counter], buf_tokens);
				counter++;
			}
			buf_tokens = strtok (NULL, " \n\t\r");
		}

		args[counter] = NULL;

		if ( counter == 0 ) {
			continue; /* need new input */
		}


/*********************************************************************************************************************
*  Exit Command
*  
*********************************************************************************************************************/

    	if (strncmp(lineEntered, "exit", 4) == 0) {
    		//printf("You want to exit\n");
    		//fflush(stdout);   // Clear buffer
    		break;
    	}


/*********************************************************************************************************************
*  CD Command
*  Checks if there is any directory name entered first, then, if there is a directory entered, checks if the directory
*  exists. If it exists, then, we change directory. 
*********************************************************************************************************************/

		else if (strncmp(lineEntered, "cd", 2) == 0) {

			if (counter == 1) {
				char *home_dir = getenv("HOME");
				chdir(home_dir);
				continue;
			}

			int chdir_status = chdir(token_arr[1]);

			// Check that the directory name is not empty...
			if(chdir_status == -1){
				printf("change directory failed to change to %s\n", token_arr[1]);
				fflush(stdout);
			} else if (chdir_status == 0) {
				//printf("successfully changed into %s\n", token_arr[1]);
				//fflush(stdout);
			} 

			continue;
		}




/*********************************************************************************************************************
*  Status Command
*  The status command prints out either the exit status or the terminating signal of the last foreground process (not 
*  both, processes killed by signals do not have exit statuses!) ran by your shell. If this command is run before any 
*  foreground command is run, then it should simply return the exit status 0. These three built-in shell commands do 
*  not count as foreground processes for the purposes of this built-in command - i.e., status should ignore built-in 
*  commands.
*  Note:  exit status of 0 is OK, 1 if minor problems, 2 if serious trouble.
*********************************************************************************************************************/

    	else if (strncmp(lineEntered, "status", 6) == 0) {
    		printf("exit value %d\n", exit_status);
    		fflush(stdout);
    	}



/*********************************************************************************************************************
*  Commented Line
*  We don't need to do anything here, just provide a new prompt once the user hits enter key.
*  
*********************************************************************************************************************/

    	else if (strncmp(lineEntered, "#", 1) == 0) {
			//printf("You entered a comment!\n");
			//fflush(stdout);   // Clear buffer
			continue;
    	}



/*********************************************************************************************************************
*  Execute Command using fork(), exec(), and waitpid().
*  Whenever a non-built in command is received, have the parent fork() off a child. This child then does any needed 
*  input/output redirection before running exec() on the command given. Note that when doing redirection, that after 
*  using dup2() to set up the redirection, the redirection symbol and redirection destination/source are NOT passed 
*  into the following exec command (i.e., if the command given is ls > junk, then you handle the redirection to "junk" 
*  with dup2() and then simply pass ls into exec() ).
*
*  Note that exec() will fail, and return the reason why, if it is told to execute something that it cannot do, like 
*  run a program that doesn't exist. In this case, your shell should indicate to the user that a command could not be 
*  executed (which you know because exec() returned an error), and set the value retrieved by the built-in status 
*  command to 1. Make sure that the child process that has had an exec() call fail terminates itself, or else it often 
*  loops back up to the top and tries to become a parent shell. This is easy to spot: if the output of the grading 
*  script seems to be repeating itself, then you've likely got a child process that didn't terminate after a failed exec().
*
*  Your shell should use the PATH variable to look for non-built in commands, and it should allow shell scripts to be 
*  executed. If a command fails because the shell could not find the command to run, then the shell will print an 
*  error message and set the exit status to 1.
* 
*  As above, after the fork() but before the exec() you must do any input and/or output redirection with dup2(). An 
*  input file redirected via stdin should be opened for reading only; if your shell cannot open the file for reading, 
*  it should print an error message and set the exit status to 1 (but don't exit the shell). Similarly, an output file 
*  redirected via stdout should be opened for writing only; it should be truncated if it already exists or created if 
*  it does not exist. If your shell cannot open the output file it should print an error message and set the exit 
*  status to 1 (but don't exit the shell).
*
*  Both stdin and stdout for a command can be redirected at the same time (see example below).
* 
*  Your program must expand any instance of "$$" in a command into the process ID of the shell itself. Your shell does 
*  not otherwise perform variable expansion. This feature makes it easier to create a grading script that keeps your 
*  work separate.
*********************************************************************************************************************/

		
		else {
			forkPID = fork();
			switch (forkPID) {
				case -1: 
					perror("Hull breach!"); 
					exit(1); 
					break;
				case 0:
					//Example from lecture 3.4: More UNIX I/O. Redirecting stdout & stdin with execlp().
					// Check input file, to see if it is named. Open the file. 
					if(strcmp(inputFileName, "")) {
						int sourceFD = open(inputFileName, O_RDONLY);
						if (sourceFD == -1) { 
							printf("cannot open %s for input\n", inputFileName); 
							fflush(stdout);
							exit(1); 
						}
						result_in = dup2(sourceFD, 0);
						if(result_in == -1) { 
							perror ("source dup2() failed\n"); 
							fflush(stdout);
							exit(2); 
						}
						// close on exec, the sourceFD file descriptor
						fcntl(sourceFD, F_SETFD, FD_CLOEXEC);
					}


					if(strcmp(outputFileName, "")) {
						int targetFD = open(outputFileName, O_WRONLY | O_CREAT | O_TRUNC, 0644);
						if (targetFD == -1) { 
							printf("cannot open %s for output\n", outputFileName); 
							fflush(stdout);
							exit(1); 
						}
						result_out = dup2(targetFD, 1);
						if (result_out == -1) { 
							perror("target dup2() failed\n"); 
							fflush(stdout);
							exit(2); 
						}
						// Close on exec, the targetFD file descriptor.
						fcntl(targetFD, F_SETFD, FD_CLOEXEC);
					}
					if( execvp(token_arr[0], (char* const*)args)) {
						perror(token_arr[0]);
						exit(2);
					}
					break;
				default:

					// Once the child process exits....
					// If user specified they want to run process in background, run in background. Note: isBackgroundProcess == 1 means yes it is a background process. 
					if (isBackgroundProcess == 1 && foreground_mode == 0) {
						background_pid = waitpid(forkPID, &childExitStatus, WNOHANG);
						printf("background pid is %d\n", forkPID);
						fflush(stdout);
						for (int i = 0; i < NBKG; i++){
							if (bkg_list[i] == UNUSED ) {
								bkg_list[i] = forkPID;
								break;
							}
						}
					} else {
						background_pid = waitpid(forkPID, &childExitStatus, 0);
					}

					break;
			}
			continue;
		}
		continue;
	}
	return 0;


}




/*********************************************************************************************************************
*  catchSIGTSTP to enter or exit foreground-only mode.
*  The following catchSIGINT function is from our course reading 3.3: Advanced User Input with getline()
*  
*********************************************************************************************************************/
void catchSIGTSTP(int signo)
{
	//If 0, we are allowed to set background processes. If set to 1, we are ONLY allowed to use foreground processes, no background processes.
	if (foreground_mode == 0) {
		char* message = "\nEntering foreground-only mode (& is now ignored)\n";
		write(STDOUT_FILENO, message, strlen(message));
		foreground_mode = 1;
	} else if (foreground_mode == 1) {
		char* message = "\nExiting foreground-only mode\n";
		write(STDOUT_FILENO, message, strlen(message));
		foreground_mode = 0;
	}
}



