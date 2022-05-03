// If you are not compiling with the gcc option --std=gnu99, then
// uncomment the following line or you might get a compiler warning
//#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h> //General OS functionality
#include <wait.h> //Waitpid
#include <sys/types.h> //pid_t
#include <sys/stat.h> // Working with FDs
#include <fcntl.h> // Working with FDs

#include "functions.h"
//#include "signals.h" //For changing signal handling of children


//Char array of builtin names and function pointer array of builtins
//	Used to cleanly check through every builtin
//	Idea from: https://brennan.io/2015/01/16/write-a-shell-in-c/
//	NOTE: POSITION MUST MATCH IN BOTH ARRAYS
//		This is how the proper function is called
char *builtinNames[] = {"cd", "exit", "status"};

int (*builtinFunArr[]) (struct shellInfo *si) = {&cdBI, &exitBI, &statusBI};

//For handling sigstp
int globalFgOnlyMode = 0;


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
		if (chdir(si->args[1]) != 0) {
			perror("Error with navigating the given directory");
		}
	}

	//fflush(stdout);


	//The 1 returned is used to signal that the shell should continue
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
	//If neither, then no foreground process has executed so
	if (si->pIfExited) {
		printf("Exit value %d\n", si->pExitStatus);
		//return si->pExitStatus;
	} else if (si->pIfSignaled){
		printf("Terminated by signal %d\n", si->pTermSignal);
		//return si->pTermSignal;
	} else {
		printf("No foreground process has run yet\n");
	}

	fflush(stdout);
	//The 1 returned is used to signal that the shell should continue
	return 1;
}



//Tries to actually execute the command given in from a command line
//	ADAPTED FROM https://brennan.io/2015/01/16/write-a-shell-in-c/
int executeCommand(struct shellInfo *si)
{
	//First, check to see if it's a background procedure
	si->isBackground = isBgProc(si);
	//Then, see if there is any redirection
	checkRedirIO(si);

	//Random Flush
	//fflush(stdout);

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
	int status = 0; //status var for the waitpid function to access
	FILE *iStream, *oStream; // For i/o redirection
	int redirFail = 0; //Flag if input or output redirection fail;

	
	pid = fork();
	if (pid == 0) {
		//Child Process
		//Now, don't ignore SIGINT if not background
		if (si->isBackground == 0) {
			si->SIGINT_act.sa_handler = &handle_SIGINT; 
			// We don't need a custom handler since it'll be overwritten anyways
			// 	But, this already works so I'm not gonna break it
			sigaction(SIGINT, &si->SIGINT_act, NULL);
		}

		//All children must ignore SIGSTP
		si->SIGSTP_act.sa_handler = SIG_IGN; 
		sigaction(SIGTSTP, &si->SIGSTP_act, NULL);

		//First, redirect I/O
		//	Background processes have /dev/null as default
		//	This is overwritten by the '<' and '>' modifiers after
		//	If a file fails to open, then set exit status to 1 and exit to shell
		if (si->isBackground == 1) {
			iStream = fopen("/dev/null", "r");
			oStream = fopen("/dev/null", "w");
			dup2(fileno(iStream), STDIN_FILENO);
			dup2(fileno(oStream), STDOUT_FILENO);
		}

		if (si->isInRedir == 1) {
			if ( !(iStream = fopen(si->inDir, "r")) ) {
				printf("Error: couldn't open file for input redirection\n");
				fflush(stdout);
				redirFail = 1;
			}
			else
				dup2(fileno(iStream), STDIN_FILENO);
		}

		if (si->isOutRedir == 1) {
			if ( !(oStream = fopen(si->outDir, "w")) ) {
				printf("Error: couldn't open file for output redirection\n");
				fflush(stdout);
				redirFail = 1;
			}
			else
				dup2(fileno(oStream), STDOUT_FILENO);
		}


		//If 'execvp' fails, print error and exit with failure
		//	Only run this if there were no redirection fails
		if (redirFail == 0)
			if (execvp(si->args[0], si->args) == -1)
				perror("Error with 'execvp' in child process");

		//Free SI on failed child process before exiting
		freeSIMembers(si);
		free(si);
		exit(1);
	} else if (pid < 0) {
		//Fork error
		perror("Error while forking");
		//RANDOM FLUSH AFTER PRINT
	} else {
		//Parent Process
		//Add child process to list IF BACKGROUND
		if (si->isBackground){
			si->bgPidList[si->bgProcessCount] = pid;
			si->bgProcessCount++;
			printf("PID of new child process: %d\n", pid);
			fflush(stdout);
		}


		//Have it wait while child executes
		//	Exit when WIFEXITED and WITSIGNALED alow`

		//IF THE PROCESS IS BACKGROUND, THEN USE THE WNOHANG CONSTANT
		if (si->isBackground) {
			do {
				wpid = waitpid(pid, &status, WNOHANG);
			} while (!WIFEXITED(status) && !WIFSIGNALED(status));
		} else {
			do {
				wpid = waitpid(pid, &status, WUNTRACED);
				si->pIfExited = WIFEXITED(status);
				si->pIfSignaled = WIFSIGNALED(status);
				si->pExitStatus = WEXITSTATUS(status);
				si->pTermSignal = WTERMSIG(status);
			} while (!WIFEXITED(status) && !WIFSIGNALED(status));

			//If a signal closed the process, then print out a message
			if (WIFSIGNALED(status))
				statusBI(si);
		}
		
	}


	//If nothing goes wrong, then return 1
	return 1;
}

int isBgProc(struct shellInfo *si) {
	int isbg = 0;
	if ( strcmp (si->args[si->argcount - 1], "&") == 0 ){
		//Frees memory in final argument so it's not treated as an argument passed into functions
		char *lastParam = si->args[si->argcount-1];
		free(lastParam);
		si->args[si->argcount-1] = NULL;
		isbg = 1;
	}

	if (globalFgOnlyMode == 1)
		isbg = 0;

	return isbg;
}


//NOTE: Because of how this checks args, it's MANDATORY to call AFTER isBgProc
void checkRedirIO(struct shellInfo *si) {
	int counter = 0;	

	//By default DON'T REDIRECT IO
	si->isInRedir = 0;
	si->isOutRedir = 0;

	//Run through the array of args 
	//	Check for either  a '>' or a '<'
	//	If they are there:
	//		Set the si struct flag for input I or O redir
	//		Have the si struct point to them 
	//		free the '<' and/or '>' arg
	//		point all values in the arg array to NULL
	//	Note: Because the output files aren't freed here, but are removed from array, they will leak
	//		Thus, they have to be cleaned by the function that frees other si struct memory
	while (si->args[counter]) {
		if (strcmp(si->args[counter], "<") == 0) {
			si->isInRedir = 1;
			si->inDir = si->args[counter+1]; // Next val in array is the wanted argument
			free(si->args[counter]);
			si->args[counter] = NULL;
			si->args[counter + 1] = NULL;
			//printf("\tNew Input Dir: %s\n\tWill there be input redir? 1 for yes: %d\n", si->inDir, si->isInRedir);

			counter++; //Basically, this will skip to either garbage or '>'
		} else if (strcmp(si->args[counter], ">") == 0) {
			si->isOutRedir = 1;
			si->outDir = si->args[counter+1]; // Next val in array is the wanted argument
			free(si->args[counter]);
			si->args[counter] = NULL;
			si->args[counter + 1] = NULL; // Points only array to NULL, prevents these args from polluting commands
			//printf("\tNew Output Dir: %s\n\tWill there be output redir? 1 for yes: %d\n", si->outDir, si->isOutRedir);

			counter++; //This will skip to either garbage or '<'
		}

		counter++;
	}

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
			fflush(stdout);
			//printf("IT HAD AN INDEX OF: %d :", i);
			if (WIFEXITED(status)) {
				printf("Exit value: %d\n", WEXITSTATUS(status));
			} else if (WIFSIGNALED(status)) {
				printf("Terminated by signal: %d\n", WTERMSIG(status));
			}

			fflush(stdout);

			//To clean, first, set the process in the list to be 0
			//	This lets the function cleanPidList work
			si->bgPidList[i] = 0;
			si->bgProcessCount--;
			cleanPidList(si);
			i--; // The array is 'dragged forward' by cleanPidList, so the iterator also must be to get every zombie
		}
	}
}



void handle_SIGINT() {
	write(1, "INT REC\n", 8);
	exit(1);

}


//int globalFgOnlyMode = 0;
int sigStopCalled = 0;

void handle_SIGSTP() {

	sigStopCalled = 1;
	//Toggle the variable to determine if the mode is on
	if (globalFgOnlyMode == 0) {
		write(1, "Entering foreground-only mode (& is now ignored)\n", 49);
		globalFgOnlyMode = 1;
	} else {
		write(1, "Exiting foreground-only mode\n", 29);
		globalFgOnlyMode = 0;
	}
	
	//exit(1);
}

