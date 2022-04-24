// If you are not compiling with the gcc option --std=gnu99, then
// uncomment the following line or you might get a compiler warning
//#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h> //Process identification

#include "assignment3.h"


//INCLUDE FUNCTION DEFINITIONS BELOW
//	ADAPTED FROM https://brennan.io/2015/01/16/write-a-shell-in-c/
void mainLoop() 
{
	//char *line; //Full line from the user
	//char **args; //Array of tokenized arguments
	int status = 1; //Whether or not to continue; Based on argument execution

	struct shellInfo *si = malloc (sizeof (struct shellInfo));
	si->shellPID = getpid();
	printf("PID of the shell: %d\n", si->shellPID);

	do {
		printf(": ");

		//Just prompt again if a comment or empty prompt is found
		si->command = getLineInput() ;
		if (lineIsValid(si->command) == 0) {
			printf("\tBlank Line or Comment inputted; please reinput;\n");
			continue;
		}

		printf("\tLegit input found\n");


		si->args = tokenizeLine(si->command);
		si->argcount = countArgs(si->args);
		printf("\tArguments counted: %d\n", si->argcount);

		//Both are allocated in called functions, not in the main loop
		free(si->command);
		free(si->args);

		status = 0;
		
	} while (status);

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
	char **tokens = malloc(ARG_BUFFER_SIZE * sizeof(char*)); //Array of bufers
	char *currToken; //Pointer to use

	//Very similar to error handling above
	if (!tokens){
		fprintf(stderr, "Failed to allocate arg buffer\n");
		exit(EXIT_FAILURE);
	}

	currToken = strtok(line, " \n\t\a\r"); //from string.h, includes common/uncommon delimiters from provided source

	//While there are still tokens in the line
	while (currToken != NULL) {
		//printf("ITERATION: %d\n, CURRTOK: %s\n", position, currToken);
		tokens[position] = currToken; //Place the tokenized value into the array of chars 
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
		//printf("Current argument: %s\nCurrent count: %d\n", currArg, counter);
	}
	while (currArg != NULL);

	return (counter - 1); // 1 for while loop, maybe 1 for ignoring the command
}
