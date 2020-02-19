/* Amanda Lawrence
*  February 4, 2019
*  CS 344 - Operating Systems
*  Project 2 - Adventure Game in C
*  
*  Room Program
*  The first thing your rooms program must do is create a directory called lawreama.rooms.<PROCESS ID OF ROOMS PROGRAM>. 
*  Next, it must generate 7 different room files, which will contain one room per file, in the directory just created. 
*  You may use any filenames you want for these 7 room files, and these names should be hard-coded into your program. 
*  The elements that make up an actual room defined inside a room file are listed below, along with some additional specifications:

*  A Room Name
*		A room name cannot be assigned to more than one room.
*		Each name can be at max 8 characters long, with only uppercase and lowercase letters allowed (thus, no numbers, special characters, or spaces). 
*		This restriction is not extended to the room file's filename.
*		You must hard code a list of ten different Room Names into your rooms program and have your rooms program randomly assign one of these to each room generated. 
*		Thus, for a given run of your rooms program, 7 of the 10 hard-coded room names will be used.
*  A Room Type
*		The possible room type entries are: START_ROOM, END_ROOM, and MID_ROOM.
*		The assignment of which room gets which type should be randomly generated each time the rooms program is run.
*		Naturally, only one room should be assigned the START_ROOM type, and only one room should be assigned the END_ROOM type. The rest of the rooms will receive the MID_ROOM type.
*  Outbound connections to other rooms
*		There must be at least 3 outbound connections and at most 6 outbound connections from this room to other rooms.
*		The oubound connections from one room to other rooms should be assigned randomly each time the rooms program is run.
*		Outbound connections must have matching connections coming back: if room A connects to room B, then room B must have a connection back to room A. 
*		Because of all of these specs, there will always be at least one path through.
*		A room cannot have an outbound connection that points to itself.
*		A room cannot have more than one outbound connection to the same room.
*/

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define MAX 100

#define MIN_FULLNESS 3

/*
*  Define structs for rooms.
*  Each struct should have a room name, room type, and pointers to be set for outbound connections to other rooms.
*/

typedef enum {START_ROOM, MID_ROOM, END_ROOM} types_of_rooms;

typedef struct room room;

struct room {
	int id;
	char* name;
	int numOutboundConnections;
	struct room* outboundConnections[6];		/* array of 3 to 6 pointers to room structs */
	types_of_rooms room_type;
};


/*
* list_of_rooms holds an array of strings, of the 7 rooms we have randomly selected.
* The array of room structs, arr_room_structs, will hold our array of room structs, where we will fill in information about each room before writing its information to a file.
*/

char list_of_rooms[7][9];

struct room arr_room_structs[MAX];

typedef enum {false = 0, true = 1} bool;				/* defines boolean values, true and false. */



/* The following defined functions are based off of reading 2.2: Program Outlining in Program 2. */


/* Returns true if all rooms have over 6 outbound connections, false otherwise. */
int i;

bool IsGraphFull(int min_conections) {
	int fewest_connections = arr_room_structs[0].numOutboundConnections;
	for(i=0; i < 7; i++){
		char temp_room_name[9];	
		snprintf(temp_room_name, sizeof(temp_room_name)-1, "%s", list_of_rooms[i]);	
		int n_connected = arr_room_structs[i].numOutboundConnections;
		if ( n_connected < fewest_connections ) { 
			fewest_connections = n_connected; 
		}
	}
	if ( fewest_connections >= min_conections ) {
		return true;
	} else {
		return false;
	}
};



/* Returns a random Room, does NOT validate if connection can be added */
room GetRandomRoom() {
	int random_num = (rand() % 7);
	return arr_room_structs[random_num];
};


int GetRandomRoomNumber() {
	int random_num = (rand() % 7);
	return random_num;
};

	
/* Returns true if a connection can be added from Room x (< 6 outbound connections), false otherwise. */
bool CanAddConnectionFrom(room *x){
	if(x->numOutboundConnections < 6){
		return true;
	} else {
		return false;
	}
};


/* Returns true if a connection from Room x to Room y already exists, false otherwise. */
bool ConnectionAlreadyExists(room *x, room *y){
	int j;

	bool exists = false;
	for(j = 0; j < x->numOutboundConnections; j++){
		if(x->outboundConnections[j]->name == y->name){
			exists = true;
		}
	}
	return exists;
};


/* Connects Rooms x and y together, does not check if this connection is valid */
void ConnectRoom(room *x, room *y){
	x->outboundConnections[x->numOutboundConnections] = y;
	x->numOutboundConnections++;
	y->outboundConnections[y->numOutboundConnections] = x;
	y->numOutboundConnections++;
};


/* Returns true if Rooms x and y are the same Room, false otherwise. */
bool IsSameRoom(room *x, room *y){
	bool isSame = false;
	if(x->name == y->name){
		isSame = true;
	}
	return isSame;
};


/* Adds a random, valid outbound connection from a Room to another Room. */
void AddRandomConnection() {
	room *A;
	room *B;

	while (true) {
		A = arr_room_structs + GetRandomRoomNumber();
		if (CanAddConnectionFrom(A) == true){
			break;
		}		
	} do {
		B = arr_room_structs + GetRandomRoomNumber();
	}
	while(CanAddConnectionFrom(B) == false || IsSameRoom(A, B) == true || ConnectionAlreadyExists(A, B) == true);
	ConnectRoom(A, B);		/* Connects these two rooms */
};




/***************************************************************************************************************************************************************************/


int main (int argc, char* argv[]){

	/*
	*  Create a directory called lawreama.rooms.<PROCESS ID OF ROOMS PROGRAM>
	*/
	char buffer[32];																	/* will store our string of lawreama.rooms.<PROCESS ID> */
	int process_id = getpid();															/* gets the process ID */
	snprintf(buffer, sizeof(buffer)-1, "lawreama.rooms.%d", process_id);				/* combines "lawreama.rooms." and the process id into one string */
	int result = mkdir(buffer, 0755);								


	/* Stores strings, of all 10 possible room names we have to choose from */
	char room_name[10][9] = {"penguin", "cat", "elephant", "sloth", "llama", "hedgehog", "turtle", "seahorse", "owl", "monkey"};




	/* Create an array of 10 integers, shuffle it, and then select the first 7 integers from this shuffled array to use as the index
 	* numbers of room names we will take from the room_name array.
 	*/

 	int indices[13];
 	srand(time(NULL));					/* seed random with time, otherwise it will generate same numbers, in same order everytime program is run */
 	int i;
 	int x;

	for (i = 0; i < 10; i++) {     /* fill indices array */
    	indices[i] = i;
	}
	for (x = 0; x < 10; x++) {    /* shuffle indices array */
    	int temp = indices[x];
    	int randomIndex = rand() % 10;
    	indices[x] = indices[randomIndex];
    	indices[randomIndex] = temp;
	}



	/* Take the first 7 values stored in the indices array, those values representing the indices of the room names we will choose from the room_name array. 
	*  This for loop simply stores our 7 room names as strings in the array list_of_rooms.
	*/
	int k;

    for (k = 0; k < 7; k++) {
    	int temp_index = indices[k];
    	strcpy(list_of_rooms[k], room_name[temp_index]);
    }



	/* 
	* Based off of the room names in the list_of_rooms array, we will create room structs for each, and store each room struct in the arr_room_structs array.
	*/
	int j;

	for (j = 0; j < 7; j++){
		struct room room_obj;
		room_obj.id = j;
		room_obj.name = calloc(16, sizeof(char));
		strcpy(room_obj.name, list_of_rooms[j]);
		room_obj.numOutboundConnections = 0;
		arr_room_structs[j] = room_obj;
	}

	/*for (int g = 0; g < 7; g++){
    *    fprintf(stderr, "arr_room_structs[%d].name: %s\n", g, arr_room_structs[g].name);
	}*/



	/* 
	*  Create all connections in graph
	*/

	int counter = 0;
	bool time_to_stop = false;
	while (time_to_stop == false) {
		counter++;
		room asdf = GetRandomRoom();
		AddRandomConnection();
		time_to_stop = IsGraphFull(MIN_FULLNESS);
		if ( counter >= 1000 ) {
			exit(EXIT_FAILURE);
		}
	}




	/*
	*	Assign each room a room type of START_ROOM, MID_ROOM, or END_ROOM.
	*
	*/
	int m;

    for (m = 0; m < 7; m++) {
    	if (m==0){
    		arr_room_structs[m].room_type = START_ROOM;
    	} else if (m > 0 && m < 6){
    		arr_room_structs[m].room_type = MID_ROOM;
    	} else {
    		arr_room_structs[m].room_type = END_ROOM;
    	}
    }




    /* Print out contents of each room struct
    *
    *for (int n = 0; n < 7; n++) {
    *	printf("n is equal to: %d\n", n);
    *	printf("ROOM NAME:  %s\n", arr_room_structs[n].name);
    *	for (int q = 0; q < arr_room_structs[n].numOutboundConnections; q++){
    *		printf("CONNECTION %d:  %s\n", q+1, arr_room_structs[n].outboundConnections[q]->name);
    *	}
    *	printf("ROOM TYPE:  %u\n", arr_room_structs[n].room_type);
    *}
	*/



	/*
	*  Generate 7 room files, one for each room, and place these files in the lawreama.rooms.<PROCESS ID OF ROOMS PROGRAM> directory created above.
	*  The filenames should be hard-coded.
	*/
	int s;
	int p;

	for(s = 0; s < 7; s++){
    	FILE *fp;
    	char file_path[40];
    	char file_name[32];
    	snprintf(file_name, sizeof(file_name)-1, "%s.room", arr_room_structs[s].name);
    	snprintf(file_path, sizeof(file_path)-1, "./%s/%s", buffer, file_name);				/* combines "./" and "lawreama.rooms.<process id>" contained in buffer and the filename into the file_path char array */
		fp = fopen(file_path, "w");
		fprintf(fp, "ROOM NAME:  %s\n", arr_room_structs[s].name);
    	for (p = 0; p < arr_room_structs[s].numOutboundConnections; p++){
    		fprintf(fp, "CONNECTION %d:  %s\n", p+1, arr_room_structs[s].outboundConnections[p]->name);
    	}
    	if (arr_room_structs[s].room_type == 0){
    		fprintf(fp, "ROOM TYPE:  %s\n", "START_ROOM");
    	} else if (arr_room_structs[s].room_type == 1){
    		fprintf(fp, "ROOM TYPE:  %s\n", "MID_ROOM");
    	} else {
    		fprintf(fp, "ROOM TYPE:  %s\n", "END_ROOM");
    	}
		fclose(fp);
	}

}