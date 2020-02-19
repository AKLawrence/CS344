/* Amanda Lawrence
*  March 11, 2019
*  CS 344: Operating Systems
*  Project 4: One-Time Pad
*  File: otp_enc_d.c
*  
*  This program connects to otp_enc and performs a one-time pad style encryption, while running in the background as a daemon. 
*  otp_enc doesn't do the encryption, otp_end_d does. otp_enc_d will run in the background, as a daemon and will take in a port
*  number specified by the user. 
*
*  This program's code is based on the file server.c provided in CS 344 - Block 4.
*  
*/




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <fcntl.h>


#define BUFFER_SIZE 100000


void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
	int charsRead2;
	socklen_t sizeOfClientInfo;
	char buffer_input[BUFFER_SIZE];
	char buffer_key[BUFFER_SIZE];
	struct sockaddr_in serverAddress, clientAddress;
	int runInBackground = 0; 	//0 is for foreground processes, and 1 is for background. 
	int pid;
	int shift_input, shift_key, cipher_output;
	char buffer_output[BUFFER_SIZE];


	if (argc < 2) { 			// Check usage & args
		fprintf(stderr,"USAGE: %s port\n", argv[1]); 
		exit(1); 
	} 
	
	if (argc == 3) {			// Check if user wants to run this in the background.
		if (strcmp(argv[2], "&")) {
			runInBackground = 1;
		}
	}


	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process
	fflush(stdout);


	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) {
		error("ERROR opening socket");
	}


	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) { 	// Connect socket to port
		fprintf(stdout, "ERROR on binding\n");
	}
	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

	while(1) {

		// Accept a connection, blocking if one is not available until one connects
		sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
		if (establishedConnectionFD < 0) {
			fprintf(stderr, "OTP_ENC_D: ERROR on accepting socket connection\n");
		}

		ntohs(clientAddress.sin_port);

		// fork, to create child process to handle:
			// check that the connection is coming from otp_enc; receiving the plaintext data and key data via establishedConnectionFD socket;
			// ciphering the text based on plaintext and key data; and writing back the cipher text to otp_enc on same establishedConnectionFD socket.
		pid = fork();

		if (pid < 0) {
			fprintf(stderr, "OTP_ENC_D: Fork failed.\n");
		}


		// Start child process
		if (pid == 0){

			// Get the plaintext file data message from the client and display it
			memset(buffer_input, '\0', BUFFER_SIZE);
			charsRead = recv(establishedConnectionFD, buffer_input, BUFFER_SIZE, 0); // Read the client's message from the socket
			if (charsRead < 0) {
				fprintf(stderr, "OTP_ENC_D: ERROR reading plaintext input from socket\n");
			}


			// Send a Success message back to the client
			charsRead = send(establishedConnectionFD, "I am the server, and I got your plaintext data message", 54, 0); // Send success back
			if (charsRead < 0) {
				fprintf(stdout, "ERROR writing to socket\n");
			}


			// Get the key file data message from the client and display it
			memset(buffer_key, '\0', BUFFER_SIZE);
			charsRead2 = recv(establishedConnectionFD, buffer_key, BUFFER_SIZE, 0); // Read the client's message from the socket
			if (charsRead2 < 0) {
				fprintf(stderr, "OTP_ENC_D: ERROR reading key data from socket\n");
			}



			// Begin ciphering text based on plaintext buffer_input data and buffer_key data.
			// Shift each character in buffer_input so their ASCII decimal values are instead between 0 and 26, including swapping ' ' to '@' first. 

			for (int k = 0; k < (strlen(buffer_input)-1); k++){
				cipher_output = 0;
				if (buffer_input[k] == ' '){
					buffer_input[k] = '@';
				}
				if (buffer_key[k] == ' '){
					buffer_key[k] = '@';
				}

				shift_input = (int)buffer_input[k];
				shift_key = (int)buffer_key[k];
				shift_input -= 64;
				shift_key -= 64;
				cipher_output = (shift_input + shift_key);// % 27;	// modulus to get our encrypted char, still in a decimal integer, from 0 to 26
				if (cipher_output >= 27){
					cipher_output -= 27;
				}
				cipher_output += 64;

				if ( (char)cipher_output == '@' ) {
					strcpy(&buffer_output[k], " ");
				} else {
					buffer_output[k] = (char) cipher_output;
				}

			}



			// Send a Success message back to the client, with the encrypted message
			charsRead = send(establishedConnectionFD, buffer_output, BUFFER_SIZE, 0); // Send success back
			if (charsRead < 0) {
				fprintf(stdout, "ERROR writing to socket\n");
			}
			close(establishedConnectionFD); // Close the existing socket which is connected to the client

		} // close while loop

	} // Closes child process
	close(listenSocketFD); // Close the listening socket
	return 0; 
}





