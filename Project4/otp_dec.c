/* Amanda Lawrence
*  March 11, 2019
*  CS 344: Operating Systems
*  Project 4: One-Time Pad
*  File: otp_dec.c
*  
*  This program connects to otp_dec_d and asks it to perform a one-time pad style decryption. By itself, otp_dec doesn't do 
*  the decryption, otp_dec_d does.
*
*  This program's code is based on the file client.c provided in CS 344 - Block 4.
*  
*/



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <signal.h>
#include <fcntl.h>


#define BUFFER_SIZE 100000


void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues


int main(int argc, char *argv[])
{
	int socketFD, portNumber, charsWritten, charsRead;
	int charsWritten2, charsRead2;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo = gethostbyname("localhost");
	char buffer_cipher[BUFFER_SIZE];
	char buffer_key[BUFFER_SIZE];
	char buffer_cipher_rec[BUFFER_SIZE];	// buffer for data received from otp_dec_d, after sending it the cipher file's buffer.
	char buffer_key_rec[BUFFER_SIZE];	// buffer for data received from otp_dec_d, after sending it the key file's buffer.
	char outputFileName[256];
	int runInBackground = 0; 	//0 is for foreground processes, and 1 is for background. 
	int cipher_fd;				// file descriptor for our cipher file
	int key_fd;					// file descriptor for our key file.
	int length_cipher;			// will keep track of length of cipher input file, for comparing to the length of key file.
	int length_key;				// will keep track of length of key file, for comparing to the length of the cipher input file. 
    int result_out;
    int i;
    int j;


	if (argc < 4) { 	// Check usage & number of args
		fprintf(stderr,"USAGE IS: %s cipherfilename keyfilename port\n", argv[0]); 
		exit(0); 
	} 



	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number

	
	if (serverHostInfo == NULL) { 
		fprintf(stderr, "OTP_DEC: ERROR, no such host\n"); 
		exit(2); 
	}

	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address
	//printf("PLAINTEXT FILENAME entered is:  %s\n", argv[1]);
	//printf("KEY FILENAME entered is:  %s\n", argv[2]);
	//printf("PORT NUMBER entered is:  %d\n", portNumber);
	fflush(stdout);

	if (argc > 4) {
		if (strcmp(argv[4], ">")) {
			if(strcmp(argv[5], "")) {
				strcpy(outputFileName, argv[5]);
				//printf("OUTPUT FILENAME entered is:  %s\n", outputFileName);
			}
		}
		if (strcmp(argv[6], "")) {
			if (strcmp(argv[6], "&")) {
				runInBackground = 1;
			}
		}
	}

	// Try to open the plaintext file the user entered, and print error to stderr if could not open it successfully.
	cipher_fd = open(argv[1], O_RDONLY);
	if (cipher_fd < 0) {
		fprintf(stderr, "Unable to open cipher file entered\n");
		exit(1);
	}


	// Try to open the key file the user entered, and print error to stderr if could not open it successfully.
	key_fd = open(argv[2], O_RDONLY);
	if (key_fd < 0) {
		fprintf(stderr, "Unable to open key file entered\n");
		exit(1);
	}


	// read in the plaintext file's contents, then check the contents for any bad characters. It should only contain uppercase letters, and the space.
	length_cipher = read(cipher_fd, buffer_cipher, BUFFER_SIZE);
	for (i = 0; i < length_cipher-1; i++) {
		if ((int) buffer_cipher[i] > 90) {
			fprintf(stderr, "OTP_DEC: ERROR. cipher input file contains bad characters\n");
			exit(1);
		} else if ( ((int)buffer_cipher[i]) < 65 && ((int) buffer_cipher[i]) != 32 ) {
			if (((int)buffer_cipher[i]) < 65 && ((int) buffer_cipher[i]) != 10) {
				fprintf(stderr, "OTP_DEC: ERROR. cipher input file contains bad characters\n");
				exit(1);
			}
		}
	}



	// read in the key file's contents, then check the contents for any bad characters. It should only contain uppercase letters, and the space.
	length_key = read(key_fd, buffer_key, BUFFER_SIZE);
	for (j = 0; j < length_key-1; j++) {
		if ((int) buffer_key[j] > 90) { 
			fprintf(stderr, "OTP_DEC: ERROR. key file contains bad characters\n");
			exit(1);
		} else if ( ((int) buffer_key[j]) < 65 && ((int) buffer_key[j]) != 32 ) { 
			if ( ((int) buffer_key[j]) < 65 && ((int) buffer_key[j]) != 10 ) {
				fprintf(stderr, "OTP_DEC: ERROR. key file contains bad characters\n");
				exit(1);
			}
		}
	}



	// check the length_cipher  vs. length_key. If length_key is shorter than length_cipher, then program should terminate, 
		// and send error to stderr, and exit(1).
	if (length_key < length_cipher) {
		fprintf(stderr, "OTP_DEC: ERROR. Key file shorter than cipher file.\n");
		exit(1);
	}

	// close both files
	close(cipher_fd);
	close(key_fd);


	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) {
		fprintf(stderr, "OTP_DEC: ERROR opening socket");
		exit(2);
	}
	

	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) { // Connect socket to address
		fprintf(stderr, "OTP_DEC: ERROR connecting to OTP_DEC_D server.\n");
		exit(2);
	}


	// Send plaintext file data in message to server
	charsWritten = send(socketFD, buffer_cipher, strlen(buffer_cipher), 0); // Write the plaintext file's buffer to the server
	if (charsWritten < 0) {
		fprintf(stderr, "OTP_DEC: ERROR writing cipher file buffer to socket");
		exit(2);
	}
	if (charsWritten < strlen(buffer_cipher)) {
		fprintf(stderr, "OTP_DEC: WARNING: Not all cipher file buffer data written to socket!\n");
		exit(2);
	}


	// Get return message from server
	memset(buffer_cipher_rec, '\0', sizeof(buffer_cipher_rec)); // Clear out the buffer again for reuse
	charsRead = recv(socketFD, buffer_cipher_rec, sizeof(buffer_cipher_rec) - 1, 0); // Read data from the socket, leaving \0 at end
	if (charsRead < 0) {
		fprintf(stderr, "OTP_DEC: ERROR reading from socket\n");
		exit(2);
	}
	//printf("OTP_ENC: I received this from the server after sending plaintext input: %s\n", buffer_cipher_rec);



	// Send key file data in message to server
	charsWritten2 = send(socketFD, buffer_key, strlen(buffer_key), 0); // Write the plaintext file's buffer to the server
	if (charsWritten2 < 0) {
		fprintf(stderr, "OTP_DEC: ERROR writing key file buffer to socket");
		exit(2);
	}
	if (charsWritten2 < strlen(buffer_key)) {
		fprintf(stderr, "OTP_DEC: WARNING: Not all key file buffer data written to socket!\n");
		exit(2);
	}


	// Get return message from server, this is the cipher text data.
	memset(buffer_key_rec, '\0', BUFFER_SIZE); // Clear out the buffer again for reuse
	charsRead2 = recv(socketFD, buffer_key_rec, BUFFER_SIZE - 1, 0); // Read data from the socket, leaving \0 at end
	if (charsRead2 < 0) {
		fprintf(stderr, "OTP_DEC: ERROR reading from socket\n");
		exit(2);
	}
	printf("%s\n", buffer_key_rec);
	fflush(stdout);




	// the buffer_key_rec buffer contains the cipher text. We will either output to stdout, or a file if user named one. 
	if (strcmp(outputFileName, "")) {
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
		// buffer_key_rec will print to file, or stdout
		printf("%s\n", buffer_key_rec);
		fflush(stdout);

		// Close on exec, the targetFD file descriptor.
		fcntl(targetFD, F_SETFD, FD_CLOEXEC);

	}


	close(socketFD); // Close the socket
	return 0;
}




