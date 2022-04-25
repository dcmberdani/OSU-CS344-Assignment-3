// If you are not compiling with the gcc option --std=gnu99, then
// uncomment the following line or you might get a compiler warning
//#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h> //Process identification

#include "cmdline.h"
#include "functions.h" //Needed to call executeCommand

//INCLUDE FUNCTION DEFINITIONS BELOW
//	ADAPTED FROM https://brennan.io/2015/01/16/write-a-shell-in-c/
void mainLoop() 
{
	int status = 1; //Whether or not to continue; Based on argument execution

	struct shellInfo *si = malloc (sizeof (struct shellInfo));
	si->shellPID = getpid();
	printf("PID of the shell: %d\n", si->shellPID);


	//TEMPORARY
	int counter = 0;

	do {
		printf(": ");

		//Just prompt again if a comment or empty prompt is found
		si->line = getLineInput() ;
		if (lineIsValid(si->line) == 0) {
			printf("\tBlank Line or Comment inputted; please reinput;\n");
			continue;
		}

		si->args = tokenizeLine(si->line);
		si->argcount = countArgs(si->args);
		expandVars(si->args);

		//AFTER THAT WORK TAKING USER INPUT, DO FUNCTIONS HERE
		status = executeCommand(si);

		//Both are allocated in called functions, not in the main loop
		freeSIMembers(si);

		
	} while (status);

	//After finishing shell, free it
	free(si);
}

//Get input from STDIN
//	ADAPTED FROM https://brennan.io/2015/01/16/write-a-shell-in-c/
char* getLineInput() {
	int position = 0;
	char *buffer = malloc (sizeof(char) * CMDLINE_BUFFER_SIZE); //Max of 2048 chars, defined as constant
	int c; //current char being read in; stored as int to not mess up EOF

	//Error handling; print to stderr if an allocation failure happens
	if (!buffer) {
		fprintf(stderr, "Failed to allocate cmd line buffer\n");
		exit(EXIT_FAILURE);
	}


	//Read stdin until we hit EOF/newLine, keeping track of the position
	//	Before eof/newline, copy the current char into the buffer
	//	At eof/newline, make the ending char null, to act as the null terminator, then reutrn
	while (1) {
		c = getchar(); //from stdio.h

		if (c == EOF)
			printf("WE HAVE RECEIVED EOF");

		if (c == EOF || c == '\n'){
			buffer[position] = '\0';
			return buffer;
		} else {
			buffer[position] = c;
		}

		position++;
	}

	//No resizing of the buffer is needed;

}

// Validates the line input, making sure there are no chars
int lineIsValid(char* line){
	if (line[0] == '\0' || line[0] == '#') {
		free(line); //Not going to be freed in the loop
		return 0;
	}
	else
		return 1;
}

//Splits the string from getLineInput into an array of tokens
//	ADAPTED FROM https://brennan.io/2015/01/16/write-a-shell-in-c/
char** tokenizeLine(char* line)
{
	int position = 0; //Current position within a token
	char **tokens = malloc(MAX_ARG_COUNT * sizeof(char*)); //Array of buffers storing arguments
	char *currToken, *tempToken; //Pointer to use

	//Very similar to error handling above
	if (!tokens){
		fprintf(stderr, "Failed to allocate arg buffer\n");
		exit(EXIT_FAILURE);
	}

	currToken = strtok(line, " \n\t\a\r"); //from string.h, includes common/uncommon delimiters from provided source

	//While there are still tokens in the line
	while (currToken != NULL) {
		tempToken = malloc(sizeof(char) * ARG_BUFFER_SIZE);
		strncpy(tempToken, currToken, ARG_BUFFER_SIZE);
		tokens[position] = tempToken;
		position++;

		currToken = strtok(NULL, " \n\t\a\r"); // strtok from NULL just continus where it left off
		
	}

	//After there are no more tokens, set the final position in the array to NULL
	tokens[position] = NULL;
	return tokens;
}

// Naive way of counting arguments read in from user
int countArgs(char** args)
{
	int counter = 0;
	char* currArg;
	do {
		currArg = args[counter];
		counter++;
	}
	while (currArg != NULL);

	return (counter - 1); // 1 for while loop, maybe 1 for ignoring the command
}

//Expands all instances of '$$' to the PID of the shell
void expandVars(char** args) {
	char* currArg;
	int counter = 0;

	char *pidStr;
	int shellPID = getpid();

	char *lPtr, *rPtr, *idPtr, *out;

	currArg = args[counter];
	while (args[counter]) {
		//If an instance of "$$" is found, place it in 'idPtr'
		while ( idPtr = strstr(currArg, "$$") ) {
			rPtr = NULL;
			//Left pointer is beginning char of string
			lPtr = currArg;

			//Right pointer is first value right of the '$$'
			if ( *(idPtr+2) != '\0' )
				rPtr = (idPtr+2);

			*idPtr = '\0'; //Set the first $ to be null terminated to allow the lPtr to properly work


			pidStr = malloc(sizeof(char) * ARG_BUFFER_SIZE);
			sprintf(pidStr, "%d", shellPID);

			out = malloc(sizeof(char) * ARG_BUFFER_SIZE);
			*out = '\0'; //Just so it initializes to smth

			//If there  is a left side, append it to final string
			//	Same with right
			//	In both cases, append the pidStr to the value
			if (lPtr != idPtr) 
				strcpy(out, lPtr);

			strcat(out, pidStr);
			
			if (rPtr)
				strcat(out, rPtr);

			

			args[counter] = out;
			free(currArg);
			free(pidStr);
			currArg = args[counter];

		}

		counter++;
		currArg = args[counter];
	}

}

