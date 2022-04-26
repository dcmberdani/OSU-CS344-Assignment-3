#ifndef __SmSINFO_H
#define __SmSINFO_H

//CONSTANTS USED THROUGH THE ASSIGNMENT
#define CMDLINE_BUFFER_SIZE 2048
#define ARG_BUFFER_SIZE 64
#define MAX_ARG_COUNT 512
#define MAX_PROCESS_COUNT 100

#include <signal.h>

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
	int status;
	int bgPidList[MAX_PROCESS_COUNT];
	int bgProcessCount;

	//Info about last ended process
	int pIfExited;
	int pExitStatus;
	int pTermSignal;
};


//FUNCTION TO HANDLE STRUCT MEMORY
void freeSIMembers(struct shellInfo *si);

void cleanPidList(struct shellInfo *si);
//SIGNAL HANDLERS
//	From C lecture
//static volatile sig_atomic_t recSIGINT = 0;
//volatile sig_atomic_t recSIGINT = 0;

//static void hijackSIGINT(int sig);
//void hijackSIGINT(int sig);

#endif
