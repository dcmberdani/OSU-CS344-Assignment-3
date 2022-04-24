#ifndef __ASSIGN3_H
#define __ASSIGN3_H

#include <string.h>
/*
 * Dev: Daniel Mendes
 * Date: 5/2/22
 */

//CONSTANTS USED BOTH IN HERE AND IN THE ASSIGNMENT
#define CMDLINE_BUFFER_SIZE 2048
#define ARG_BUFFER_SIZE 16
#define MAX_ARG_COUNT 512
#define MAX_PROCESS_COUNT 100

//STRUCT THAT STORES ALL INFORMATION
struct shellInfo {

	//Info from the command line
	char* line;
	char* command; //This is the first argument
	char** args;
	int argcount;

	//Other stuff to use later; Maybe in separate struct?
	//Specifioc Process Info
	int canBackground;
	int isBackground;	
	char currentDir[100];

	//General process info
	int shellPID;
	int pidList[MAX_PROCESS_COUNT];
	int processCount;
	int processStatus;
};


//FUNCTION DECLARATIONS BELOW

void mainLoop();

char* getLineInput();

int lineIsValid(char* line);

char** tokenizeLine(char* line);

int countArgs(char** args);

void expandVars(char** args);

void freeSIMembers(struct shellInfo *si);

#endif
