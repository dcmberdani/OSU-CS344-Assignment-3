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
	char** args;
	int argcount;

	//Shell info
	int shellPID;
	int status; // This isn't for foreground processes, but for the shell itself

	//Info to determine i/o redirection
	int isInRedir;
	int isOutRedir;
	char *inDir;
	char *outDir;

	//Background process info
	int isBackground; // used to determine if the next process is to be background
	int bgPidList[MAX_PROCESS_COUNT];
	int bgProcessCount;

	//Info about last ended process
	int pIfExited;
	int pIfSignaled;
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
