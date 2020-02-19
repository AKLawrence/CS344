/* Amanda Lawrence
*  February 4, 2019
*  CS 344 - Operating Systems
*  Project 2 - Adventure Game in C
*  
*  The Game Program
*  When compiled with the same name (minus the extension) and run provides an interface for playing the game using the most recently generated rooms.
*  
*  In the game, the player will begin in the "starting room" and will win the game automatically upon entering the "ending room", 
*  which causes the game to exit, displaying the path taken by the player.
*  
*  During the game, the player can also enter a command that returns the current time - this functionality utilizes mutexes and multithreading. 
*
*  *** Use Raw C (C89), not C99 standard. So do not use the -c99 flag when compiling. ***
*
*
*  Once the rooms program has been run, which generates the room files, the game program can then be run. This program should present an interface to the player. 
*  Note that the room data must be read back into the program from the previously-generated room files, for use by the game. 
*  Since the rooms program may have been run multiple times before executing the game, your game should use the most recently created files: perform a stat() function call. 
*  on rooms directories in the same directory as the game, and open the one with most recent st_mtime component of the returned stat struct.
*  
*  This player interface should list where the player current is, and list the possible connections that can be followed. It should also then have a prompt. Here is the form that must be used:
*		CURRENT LOCATION: XYZZY
*  		POSSIBLE CONNECTIONS: PLOVER, Dungeon, twisty.
*  		WHERE TO? >
*
*  The cursor should be placed just after the > sign. Note the punctuation used: colons on the first two lines, commas on the second line, and the period on the second line. All are required.
*  When the user types in the exact name of a connection to another room (Dungeon, for example), and then hits return, your program should write a new line, and then continue running as before. 
*  For example, if I typed twisty above, here is what the output should look like:
*  		CURRENT LOCATION: XYZZY
*  		POSSIBLE CONNECTIONS: PLOVER, Dungeon, twisty.
*  		WHERE TO? >twisty
*
*  		CURRENT LOCATION: twisty
*  		POSSIBLE CONNECTIONS: PLOVER, XYZZY, Dungeon, PLUGH.
*  		WHERE TO? >
*
*

*  If the user types anything but a valid room name from this location (case matters!), the game should return an error line that says “HUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.”, and repeat the current location and prompt, as follows:

*  		CURRENT LOCATION: XYZZY
*  		POSSIBLE CONNECTIONS: PLOVER, Dungeon, twisty.
*  		WHERE TO? >Twisty

*  		HUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.

* 		CURRENT LOCATION: XYZZY
*		POSSIBLE CONNECTIONS: PLOVER, Dungeon, twisty.
*		WHERE TO? >

*  Trying to go to an incorrect location does not increment the path history or the step count. Once the user has reached the End Room, the game should indicate that it has been reached. 
*  It should also print out the path the user has taken to get there, the number of steps taken (not the number of rooms visited, which would be one higher because of the start room), 
*  a congratulatory message, and then exit.

*  Note the punctuation used in the complete example below: we're looking for the same punctuation in your program.

*  When your program exits, set the exit status code to 0, and leave the rooms directory in place, so that it can be examined.

*  If you need to use temporary files (you probably won't), place them in the directory you create, above. Do not leave any behind once your program is finished. We will not test for early termination of your program, so you don’t need to watch for those signals.



*  Time Keeping

*  Your game program must also be able to return the current time of day by utilizing a second thread and mutex(es). I recommend you complete all other portions of this assignment first, 
*  then add this mutex-based timekeeping component last of all. Use the classic pthread library for this simple multithreading, which will require you to use the "-lpthread" compile option 
*  switch with gcc (see below for compilation example).

*  When the player types in the command "time" at the prompt, and hits enter, a second thread must write the current time in the format shown below (use strftime(). 
*  to a file called "currentTime.txt", which should be located in the same directory as the game. The main thread will then read this time value from the file and print it out to the user, 
*  with the next prompt on the next line. I recommend you keep the second thread running during the execution of the main program, and merely wake it up as needed via this "time" command. 
*  In any event, at least one mutex must be used to control execution between these two threads.

*  To help you know if you're using it right, remember these key points about mutexes:

*  They're only useful if ...lock() is being called in multiple threads (i.e. having the only ...lock() and ...unlock() calls occur in one thread isn't using the mutex for control).
*  Mutexes allow threads to jockey for contention by having multiple locks (attempt to be) established.
*  A thread can be told to wait for another to complete with the ...join() function, resuming execution when the other thread finally terminates.
*  Using the time command does not increment the path history or the step count. Do not delete the currentTime.txt file after your program completes - it will be examined by the grading TA.

*  Here is an example of the "time" command being run in-game:

*		CURRENT LOCATION: XYZZY
*		POSSIBLE CONNECTIONS: PLOVER, Dungeon, twisty.
*		WHERE TO? >time

*		 1:03pm, Tuesday, September 13, 2016

*		WHERE TO? >
*		Complete Example

*  Here is a complete example of the game being compiled and run, including the building of the rooms. Note the time command, incorrect spelling of a room name, 
*  the winning messages and formatting, and the return to the prompt with some examination commands following:

*		$ gcc -o smithj.buildrooms smithj.buildrooms.c
*		$ smithj.buildrooms
*		$ gcc -o smithj.adventure smithj.adventure.c -lpthread
*		$ smithj.adventure
*		CURRENT LOCATION: XYZZY
*		POSSIBLE CONNECTIONS: PLOVER, Dungeon, twisty.
*		WHERE TO? >Twisty

*		HUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.

*		CURRENT LOCATION: XYZZY
*		POSSIBLE CONNECTIONS: PLOVER, Dungeon, twisty.
*		WHERE TO? >time

*		 1:03pm, Tuesday, September 13, 2016

*		WHERE TO? >twisty

*		CURRENT LOCATION: twisty
*		POSSIBLE CONNECTIONS: PLOVER, XYZZY, Dungeon, PLUGH.
*		WHERE TO? >Dungeon

*		YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!
*		YOU TOOK 2 STEPS. YOUR PATH TO VICTORY WAS:
*		twisty
*		Dungeon
*		$ echo $?
*		0
*		$ ls
*		currentTime.txt smithj.adventure smithj.adventure.c smithj.buildrooms
*		smithj.buildrooms.c smithj.rooms.19903
*		$ ls smithj.rooms.19903
*		Crowther_room Dungeon_room PLUGH_room PLOVER_room
*		twisty_room XYZZY_room Zork_room
*		$ cat smithj.rooms.19903/XYZZY_room
*		ROOM NAME: XYZZY
*		CONNECTION 1: PLOVER
*		CONNECTION 2: Dungeon
*		CONNECTION 3: twisty
*		ROOM TYPE: START_ROOM

*
*
*/


#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <pthread.h>


#define MAX 7


/*pthread_mutex_t mutex;*/
pthread_mutex_t lock; 

typedef enum {START_ROOM, MID_ROOM, END_ROOM} types_of_rooms;

typedef struct room room;

struct room {
	int id;
	char* name;
	int numOutboundConnections;
	char strOutboundConnections[6][9];			/* array to hold the strings of the room names it connects to */
	struct room* outboundConnections[6];		/* array of 3 to 6 pointers to room structs */
	types_of_rooms room_type;
};

struct room arr_room_structs[MAX];

typedef enum {false = 0, true = 1} bool;                /* defines boolean values, true and false. */


static
void timeWrite(void *folder_path){
    pthread_mutex_lock(&lock);              /* Allows our time_thread to lock the lock mutex. */

    /* create time file and read from it */
    FILE* timeFile;
    char full_path[100];
    snprintf(full_path, sizeof(full_path), "%s%s", (char *)folder_path, "/currentTime.txt");
    timeFile = fopen(full_path, "w");
    char buf[100];
    struct tm *my_tm;
  
    time_t time_right_now = time (0);
    my_tm = gmtime(&time_right_now);
  
    strftime (buf, sizeof(buf), "%H:%M%p, %A, %B %d, %Y", my_tm);
    fputs(buf, timeFile);
    fclose(timeFile);

    pthread_mutex_unlock(&lock);            /* unlocks lock mutex for main to use and lock */
    return;
}

static
void timeRead(void *folder_path){
    FILE* timeFile;
    char full_path[100];
    snprintf(full_path, sizeof(full_path), "%s%s", (char *)folder_path, "/currentTime.txt");
    timeFile = fopen(full_path, "r");
    char buf[100];

    /* Print out the time to the console */
    if(timeFile == NULL){
        fprintf(stderr, "CurrentTime.txt file not found.\n");
    } else {
        fgets(buf, 50, timeFile);
        printf("\n%s\n", buf);
    }
    fclose(timeFile);
    return;

}




int main() {

	/* Following code for finding our most recently created directory is based on reading 2.4 Manipulating Directories */

	int newestDirTime = -1; /* Modified timestamp of newest subdir examined */
	char targetDirPrefix[32] = "lawreama.rooms."; /* Prefix we're looking for */
	char newestDirName[256]; /*  Holds the name of the newest dir that contains prefix */
	memset(newestDirName, '\0', sizeof(newestDirName));
    bool should_print_time = false;         /* Used when getting user input, to determine whether we will prompt main() thread to print time, once our second thread creates currentTime.txt */



	DIR* dirToCheck; 			/* Holds the directory we're starting in */
	struct dirent *fileInDir; 	/* Holds the current subdir of the starting dir */
	struct stat dirAttributes; 	/* Holds information we've gained about subdir */

	dirToCheck = opendir("."); 											/* Open up the directory this program was run in */

	if (dirToCheck > 0) 												/* Make sure the current directory could be opened */
	{
    	while ((fileInDir = readdir(dirToCheck)) != NULL) {				/* Check each entry in dir */
      		if (strstr(fileInDir->d_name, targetDirPrefix) != NULL) {	/* If entry has prefix */
        		stat(fileInDir->d_name, &dirAttributes); 				/* Get attributes of the entry */
        		if ((int)dirAttributes.st_mtime > newestDirTime) {		/* If this time is bigger */
          			newestDirTime = (int)dirAttributes.st_mtime;
          			memset(newestDirName, '\0', sizeof(newestDirName));
          			strcpy(newestDirName, fileInDir->d_name);
        		}
      		}
    	}
	}
	closedir(dirToCheck); 	/* Close the directory we opened */


	char my_path[100];
	snprintf(my_path, sizeof(my_path)-1, "%s%s", "./", newestDirName);		/* my_path = ./lawreama.rooms.<process id> */




    /* Initialize our lock mutex, and then lock it for main thread to use. */
    pthread_mutex_init(&lock, NULL);
    pthread_mutex_lock(&lock);
    pthread_t time_thread;
    pthread_create(&time_thread, NULL, &timeWrite, my_path); 











	struct dirent *file_in_dir;						/* pointer to a file in our lawreama.rooms.<process id> directory */
	DIR *dr = opendir(my_path); 

	char* path_and_filename[100];
	char* startroom_filepath[100];
	int file_descriptor;
	char readBuffer[256];
	ssize_t nread, nwritten;


  
    if (dr == NULL) {								 		/* opendir returns NULL if couldn't open directory */
        printf("Could not open current directory" ); 
        return 1; 
    } 



    /* Opens up each of the room files and retrieves their contents.
    *  Puts their contents, room name, outbound connections, and room type into a room struct, 
    *  similar to the room struct defined in the lawreama.buildrooms.c file.
    * 
    */
    char* buf[100]; 
    int lines = 0;
    int line_counter = 0;
    char *lines_from_readBuffer;		/* This will store the content from readBuffer, tokenized on \n, in order to parse each line from the room file. */
    char room_name[20];
    char detect1[20];
    char detect2[20];
    char detect3[20];
    int x;

    /* Sets up a room object to be populated once we retrieve data from our files. These structs will be stored in the arr_room_structs[MAX] */
	int room_id = 0;



/**************************************************************************************************************************************************************
*   The following is setting up the access to our lawreama.rooms.<process id> directory, opening up the 7 room files in our directory, and then reading 
*   all the data from these files and storing it in structs for us to use during game play. 
*
**************************************************************************************************************************************************************/


    //fprintf(stderr, "Got to this point ...\n");
    //char prefix[32] = '\0';
    //char suffix[32] = '\0';
    //char animal[64] = { '\0' };
    int name_length = 0;
    char *subword = NULL;

	/* Open up the files, read in each line and divy up the room name, connection names, and room type to put into a struct */
    while ((file_in_dir = readdir(dr)) != NULL){

    	/* Sets up a room object to be populated once we retrieve data from our files. These structs will be stored in the arr_room_structs[MAX] */
		struct room room_obj;
		room_obj.name = calloc(16, sizeof(char));
		room_obj.numOutboundConnections = 0;
		room_obj.id = room_id;


    	lines = 0;
    	line_counter = 0;
    	snprintf(path_and_filename, sizeof(path_and_filename), "%s%s%s", my_path, "/", file_in_dir->d_name);
        /* fprintf(stderr, "\nNext path_and_filename: %s (room_id=%d)\n", path_and_filename, room_id); */
        name_length = strlen(file_in_dir->d_name);
        if ( file_in_dir->d_name[0] == '.' ) {
            /* fprintf(stderr, "Found a folder: %s\n", file_in_dir->d_name); */
            continue;
        }

        /* Make sure the filename ends with .room: */
        subword = file_in_dir->d_name + name_length - 5;
        if ( strcmp(subword, ".room") != 0 ) {
            /* fprintf(stderr, "IMPOSTOR!!!\n"); */
            continue;
        }
        
/*
        fprintf(stderr, "ANIMAL DETECTED!\n");
        fprintf(stderr, "nparsed: %d\n", nparsed);
        //fprintf(stderr, "animal: %s\n", animal);
        fprintf(stderr, "subword: %s\n", subword);
        fprintf(stderr, "\n\n");
  */
    	file_descriptor = open(path_and_filename, O_RDWR, S_IRUSR);
    	if (file_descriptor == -1){
            /*
            printf("\nCurrent room_id: %d\n", room_id);
    		printf("opening this file failed at %s\n", path_and_filename);
    		perror("Opening room file failed");
            */
    	} else {
    		memset(readBuffer, '\0', sizeof(readBuffer));
    		lseek(file_descriptor, 0, SEEK_SET);				/* Reset file pointer to beginning of file*/
    		nread = read(file_descriptor, readBuffer, sizeof(readBuffer));

    	    lines_from_readBuffer = strtok(readBuffer, "\n");

            int count = 0;
            while (lines_from_readBuffer != NULL){
                sscanf(lines_from_readBuffer, "%s %s %s", detect1, detect2, detect3);	/* Splits up a single line into 3 tokens */

                /* Populates our room object with the data from the current file */
                if (strcmp(detect2, "NAME:") == 0){									/* If we find "NAME:" in the second token on a line.... */
                    strcpy(room_obj.name, detect3);									/*  assign its third token to the room name. */
                } else if (strcmp(detect1, "CONNECTION") == 0){						/* If we find "CONNECTION" in the first token on a line... */
                    strcpy(room_obj.strOutboundConnections[room_obj.numOutboundConnections], detect3);		/*  assign its third token to the char array strOutboundConnections and increment numOutboundConnections */
                    room_obj.numOutboundConnections++;
                } else if (strcmp(detect2, "TYPE:") == 0) {
                    if (strcmp(detect3, "START_ROOM") == 0){
                        room_obj.room_type = START_ROOM;
                    } else if (strcmp(detect3, "MID_ROOM") == 0) {
                        room_obj.room_type = MID_ROOM;
                    } else if (strcmp(detect3, "END_ROOM") == 0) {
                        room_obj.room_type = END_ROOM;
                    }
                }
                lines_from_readBuffer = strtok(NULL, "\n");
                count++;
            }
    	    arr_room_structs[room_id] = room_obj;
            room_id++;
    	}
    } 

    /* Finished with file I/O from ./lawreama.rooms.<process ID> */
    closedir(dr);

    /*
    fprintf(stderr, "\n\nAfter loading room files, we have:\n");
    for ( x = 0; x < 7; x++ ) {
        fprintf(stderr, "--------------------------------------------\n");
        fprintf(stderr, "arr_room_structs[%d].name:       %s\n", x, arr_room_structs[x].name);
        fprintf(stderr, "arr_room_structs[%d].room_type = %d\n", x, arr_room_structs[x].room_type);
        fprintf(stderr, "arr_room_structs[%d].numOutboundConnections: %d\n", x, arr_room_structs[x].numOutboundConnections);
        for ( int j = 0; j < arr_room_structs[x].numOutboundConnections; j++ ) {
            fprintf(stderr, "arr_room_structs[%d].strOutboundConnections[%d]: %s\n", x, j, arr_room_structs[x].strOutboundConnections[j]);
        }
    }
    */

    /* Earlier, we only read in the names of Outbound Connections as strings, and stored them in room_obj.strOutboundConnections[] array of strings.
    *  Now that all the structs have been created, we need to create the struct room pointers and populate room_obj->outboundConnection[] array of pointers
    *  We also set the struct room pointer called currentRoom to the START_ROOM.
    */

    struct room *currentRoom;
    int total_rooms;
    int cur_connection;
    int i;



    for (total_rooms = 0; total_rooms < 7; total_rooms++) {
    	for(cur_connection = 0; cur_connection < arr_room_structs[total_rooms].numOutboundConnections; cur_connection++) {
            if( arr_room_structs[total_rooms].room_type == START_ROOM ) {
                currentRoom = &arr_room_structs[total_rooms];
            }
    		for (i = 0; i < 7; i++) {
    			if (strcmp (arr_room_structs[total_rooms].strOutboundConnections[cur_connection], arr_room_structs[i].name) == 0) {
    				arr_room_structs[total_rooms].outboundConnections[cur_connection] = &arr_room_structs[i];
                    break;
                }
    		}	
    	}
    }





/**************************************************************************************************************************************************************
*   The following is setting up the user interface to accept user input for our game
*
*
**************************************************************************************************************************************************************/


    /* Print out current location, and possible connections, and let the console wait for the user to input their next move. 
    * Then, evaluate the validity of their input, and either, print out "HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN." if it is not a valid room or connection, 
    * or move currentRoom pointer to the new room. Check if this is the END_ROOM, and if so, game is finished. 
    */

    char user_input[20];                /* To store the room name entered by the user. */
    bool valid_connection = false;
    bool is_end_room = false;
    int step_count = 0;
    char path_to_end[100][9];
    int g;
    int y;
    int z;


    while(true) {

        /*
            if ( should_print_time == true ) {
                1. print_stored_time();
                1. read time from currentTime.txt
                2. print that time
                3. set should_print_time = false    
            }
        */

        if (should_print_time == true) {
            timeRead(my_path);
            printf("\n");
            should_print_time = false;
        }



        if (currentRoom->room_type == END_ROOM){
            strcpy(path_to_end[step_count], currentRoom->name);
            printf("CONGRATULATIONS! YOU HAVE FOUND THE END ROOM!\n");
            printf("YOU HAVE TAKEN %d STEPS.\n", step_count);
            printf("THE PATH YOU TOOK IS:  ");
            for (g = 0; g < step_count+1; g++){
                printf("%s  ", path_to_end[g]);
            }
            printf("\n");
            exit (EXIT_SUCCESS);
        }


        printf("CURRENT LOCATION:  %s\n", currentRoom->name);
        strcpy(path_to_end[step_count], currentRoom->name);
        printf("POSSIBLE CONNECTIONS: ");
        /*
        fprintf(stderr, "POSSIBLE STDERR CONNECTIONS: ");
        fprintf(stderr, "\nn_outbound: %d\n", currentRoom->numOutboundConnections);

        fprintf(stderr, "\n\nOutbound connection pointers:\n");
        for (x = 0; x < currentRoom->numOutboundConnections; x++) {
            fprintf(stderr, "currentRoom->outboundConnections[%d]: %p\n", x, currentRoom->outboundConnections[x]);
        }
        fprintf(stderr, "\n\n\n\n");
        */
        for (y = 0; y < currentRoom->numOutboundConnections; y++) {
            /*
            fprintf(stderr, "\nx=%d\n", x);
            fprintf(stderr, "%s", currentRoom->outboundConnections[x]->name);
            */
            printf("%s", currentRoom->outboundConnections[y]->name);
            if (y < currentRoom->numOutboundConnections-1){
                printf(", ");
            } else if (y == currentRoom->numOutboundConnections-1){
                printf(".\n");
            }
        }
        /*fprintf(stderr, "JUST BEFORE PROMPT ...\n");*/
        printf("WHERE TO? >");
        scanf("%s", user_input);
        /*fprintf(stderr, "You just entered  %s\n", user_input);*/

        /* check for 'time' case here. If found, handle and CONTINUE */
        /*
            1. set should_print_time = true
            2. unlock mutex --> time-thread thread updates currentTime.txt
            3. join() --> pauses until time-thread returns
            4. lock() --> prevent time-thread from running further
            5. continue to next loop iteration
            */

        if (strcmp(user_input, "time") == 0) {
            pthread_mutex_unlock(&lock); 
            pthread_join(time_thread, NULL);
            pthread_mutex_lock(&lock);
            pthread_create(&time_thread, NULL, &timeWrite, my_path);
            should_print_time = true;
            continue;
        }

        /*fprintf(stderr, "Past check for 'time' command.\n");*/



        /*Check that the room is one of outbound connections... */
        int z;


        valid_connection = false;
        /*fprintf(stderr, "Current/previous room has %d outbound connections.\n", currentRoom->numOutboundConnections);*/
        for(z = 0; z < currentRoom->numOutboundConnections; z++) {
            /*fprintf(stderr, "z=%d\n", z);*/
            if (strcmp(user_input, currentRoom->outboundConnections[z]->name) == 0) {    /* If the user_input is a valid outbound connect, we set currentRoom pointer equal to this connected room. */
                valid_connection = true;
                printf("We are now moving on to the next room:  %s\n", currentRoom->outboundConnections[z]->name);
                step_count++;
                currentRoom = currentRoom->outboundConnections[z];
            }
            if ( valid_connection == true ) break;
        }
        if (valid_connection == false){
            printf("\n");
            printf("HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
            printf("\n");
        }
        valid_connection = false;

        /* fprintf(stderr, "reached end of while loop, starting next iteration.\n"); */

    }
    return 0;
}


