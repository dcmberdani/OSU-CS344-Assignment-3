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
	printf("DO CD CALL HERE;\n");

	//If the arguments array has only a command, write an error
	//	Otherwise, use chdir to enter the given directory
	if (si->args[1] == NULL) {
		fprintf(stderr, "Expected argument to 'cd'\n");
	} else {
		if (chdir(si->args[1]) != 0)
			perror("Error with navigating the given directory");
	}

	//The 1 returned is used to signal the 'status' flag that the shell should continue
	return 1;
}

int exitBI(struct shellInfo *si)
{
	printf("DO EXIT CALL HERE;\n");

	return 0; //Signals to stop running
}

int statusBI(struct shellInfo *si)
{
	printf("DO STATUS CALL HERE;\n");
	return 1;
}



//Tries to actually execute the command given in from a command line
//	ADAPTED FROM https://brennan.io/2015/01/16/write-a-shell-in-c/
int executeCommand(struct shellInfo *si)
{
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

		exit(EXIT_FAILURE);
	} else if (pid < 0) {
		//Fork error
		perror("Error while forking");
	} else {
		//Parent Process
		//Have it wait while child executes
		//	Exit when WIFEXITED and WITSIGNALED alow`
		do {
			wpid = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}

	//If nothing goes wrong, then return 1
	return 1;
}
