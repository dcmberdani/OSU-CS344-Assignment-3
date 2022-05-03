#ifndef __SmSINFO_H
#define __SmSINFO_H

//CONSTANTS USED THROUGH THE ASSIGNMENT
#define CMDLINE_BUFFER_SIZE 2048
#define ARG_BUFFER_SIZE 64
#define MAX_ARG_COUNT 512
#define MAX_PROCESS_COUNT 100

#include <signal.h>

//STRUCT THAT STORES ALL INFORMATION FOR THE SHELL
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

	//Signal Handlers
	struct sigaction SIGINT_act;
	struct sigaction SIGSTP_act;
};


//FUNCTION TO HANDLE STRUCT MEMORY
/*
 * Name: freeSIMembers
 * Description: Frees the members of the shellInfo struct that have dynamic memory
 * 	This includes the line input, the tokenized line array, and the redirection directories
 * Preconditions: An SI struct with allocated memory
 * Postconditions: An SI struct with freed memory
 */
void freeSIMembers(struct shellInfo *si);

/*
 * Name: cleanPidList
 * Description: re-organizes the PID list
 * 	'slides down' all indices on the PID list so that they all lie on the earliest indices
 * 	This is done to prevent bugs when removing zombie children
 * Preconditions: a PID list with some members
 * Postconditions: a reorganized PID list
 */
void cleanPidList(struct shellInfo *si);

#endif
