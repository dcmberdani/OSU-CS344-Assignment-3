#ifndef __SmSINFO_H
#define __SmSINFO_H

//CONSTANTS USED THROUGH THE ASSIGNMENT
#define CMDLINE_BUFFER_SIZE 2048
#define ARG_BUFFER_SIZE 16
#define MAX_ARG_COUNT 512
#define MAX_PROCESS_COUNT 100


//STRUCT THAT STORES ALL INFORMATION
struct shellInfo {

	//Info from the command line
	char* line;
	//char* command; //This is the first argument
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


#endif
