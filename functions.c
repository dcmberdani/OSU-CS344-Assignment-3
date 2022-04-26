// If you are not compiling with the gcc option --std=gnu99, then
// uncomment the following line or you might get a compiler warning
//#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h> //General OS functionality
#include <wait.h> //Waitpid
#include <sys/types.h> //pid_t

#include "functions.h"


//Char array of builtin names and function pointer array of builtins
//	Used to cleanly check through every builtin
//	Idea from: https://brennan.io/2015/01/16/write-a-shell-in-c/
//	NOTE: POSITION MUST MATCH IN BOTH ARRAYS
//		This is how the proper function is called
char *builtinNames[] = {"cd", "exit", "status"};

int (*builtinFunArr[]) (struct shellInfo *si) = {&cdBI, &exitBI, &statusBI};


//JUST TO TEST
void printArgs(struct shellInfo *si)
{
	int i = 0;
	printf("Args being executed: ");
	while (si->args[i]){
		printf(si->args[i]);
		i++;
	}
	printf("\n");
}


//BUILT IN FUNCTIONS
//	ADAPTED FROM: //brennan.io/2015/01/16/write-a-shell-in-c/
int cdBI(struct shellInfo *si)
{
	//If the arguments array has only the command cmd, nagivate to 'HOME' environment variable
	//	Otherwise, use chdir to enter the given directory
	char *homedir;
	homedir = getenv("HOME");
	if (si->args[1] == NULL) {
		chdir(homedir);
	} else {
		if (chdir(si->args[1]) != 0)
			perror("Error with navigating the given directory");
	}

	//The 1 returned is used to signal the 'status' flag that the shell should continue
	return 1;
}

int exitBI(struct shellInfo *si)
{
	return 0; //Signals to stop running
}

int statusBI(struct shellInfo *si)
{
	//If the process exited, then print the exit status
	//If the process was closed by a sginal, print the terminating signal
	if (si->pIfExited)
		printf("exit value %d\n", si->pExitStatus);
	else
		printf("terminated by signal %d\n", si->pTermSignal);
	return si->status;
}



//Tries to actually execute the command given in from a command line
//	ADAPTED FROM https://brennan.io/2015/01/16/write-a-shell-in-c/
int executeCommand(struct shellInfo *si)
{
	//First, check to see if it's a background procedure
	si->isBackground = isBgProc(si);

	//printArgs(si);	
	//If the command passed in matches a builtin, then call it
	for (int i = 0; i < NUM_BUILTINS; i++){
		if (strcmp(si->args[0], builtinNames[i]) == 0)
			return (*builtinFunArr[i])(si);
	}

	//Otherwise, fork/exec to execute non-built-ins

	
	return executeNonBI(si);
	//return 1;
}

//Again, https://brennan.io/2015/01/16/write-a-shell-in-c/
//	Uses execvp to execute things within the 'PATH' variable
int executeNonBI(struct shellInfo *si) 
{
	pid_t pid; //pid for current process
	pid_t wpid; //pid for waiting parent process while child process executes in foreground
	int status; //status var for the waitpid function to access
	
	pid = fork();
	if (pid == 0) {
		//Child Process
		//	if 'execvp' fails, print error and exit with failure
		if (execvp(si->args[0], si->args) == -1)
			perror("Error with 'execvp' in child process");
		
		//Free SI on failed child process before exiting
		freeSIMembers(si);
		free(si);
		exit(1);
	} else if (pid < 0) {
		//Fork error
		perror("Error while forking");
	} else {
		//Parent Process
		//Add child process to list IF BACKGROUND
		if (si->isBackground){
			si->bgPidList[si->bgProcessCount] = pid;
			si->bgProcessCount++;
			printf("PID of new child process: %d\nCurrent count of background processes: %d\n", pid, si->bgProcessCount);
		}

		//Have it wait while child executes
		//	Exit when WIFEXITED and WITSIGNALED alow`

		//IF THE PROCESS IS BACKGROUND, THEN USE THE WNOHANg CONSTANT
		if (si->isBackground) {
			do {
				wpid = waitpid(pid, &status, WNOHANG);
				si->pIfExited = WIFEXITED(status);
				si->pExitStatus = WEXITSTATUS(status);
				si->pTermSignal = WTERMSIG(status);
			} while (!WIFEXITED(status) && !WIFSIGNALED(status));
		} else {
			do {
				wpid = waitpid(pid, &status, WUNTRACED);
				si->pIfExited = WIFEXITED(status);
				si->pExitStatus = WEXITSTATUS(status);
				si->pTermSignal = WTERMSIG(status);
			} while (!WIFEXITED(status) && !WIFSIGNALED(status));
		}
		
	}

	//If nothing goes wrong, then return 1
	return 1;
}

int isBgProc(struct shellInfo *si) {
	if ( strcmp (si->args[si->argcount - 1], "&") == 0 ){
		//Frees memory in final argument so it's not treated as an argument passed into functions
		char *lastParam = si->args[si->argcount-1];
		free(lastParam);
		si->args[si->argcount-1] = NULL;
		return 1;
	}
	return 0;
}


void cleanUpZombies(struct shellInfo *si) {
	pid_t zomPid;
	int status;

	//Run through the list of processes
	//	waitpid at all the listed processes with WNOHANG
	//		If 0 is returned, then the child process is still running
	//		If not, then grab the exit/termination code and move on
	//	Print out their exit codes
	for (int i = 0; i < si->bgProcessCount; i++) {
		zomPid = waitpid(si->bgPidList[i], &status, WNOHANG);	

		//If it's nonzero, print a message saying it's cleaned up
		//	Check to see if it exited/was signaled to end 
		//	Then, also decrement the count of processes

		

		//NOTE: NEED SOME FORM OF ARRAY CLEANUP
		//	Honestly it's probably just better to use a linked list
		//	but ehhhhhhhhhh sounds like more memory management that im not tryna do
		if (zomPid != 0) {
			printf("Background process w/PID '%d' is done: ", si->bgPidList[i]);
			//printf("IT HAD AN INDEX OF: %d :", i);
			if (WIFEXITED(status)) {
				printf("Exit value: %d\n", WEXITSTATUS(status));
			} else if (WIFSIGNALED(status)) {
				printf("Terminated by signal: %d\n", WTERMSIG(status));
			}

			//To clean, first, set the process in the list to be 0
			//	This lets the function cleanPidList work
			si->bgPidList[i] = 0;
			si->bgProcessCount--;
			cleanPidList(si);
		}
	}
}
