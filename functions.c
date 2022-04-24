// If you are not compiling with the gcc option --std=gnu99, then
// uncomment the following line or you might get a compiler warning
//#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h> //General OS functionality

#include "functions.h"


//Char array of builtin names and function pointer array of builtins
//	Used to cleanly check through every builtin
//	Idea from: https://brennan.io/2015/01/16/write-a-shell-in-c/
//	NOTE: POSITION MUST MATCH IN BOTH ARRAYS
//		This is how the proper function is called
char *builtinNames[] = {"cd", "exit", "status"};

int (*builtinFunArr[]) (struct shellInfo *si) = {&cdBI, &exitBI, &statusBI};




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
	//If the command passed in matches a builtin, then call it
	for (int i = 0; i < NUM_BUILTINS; i++){
		if (strcmp(si->args[0], builtinNames[i]) == 0)
			return (*builtinFunArr[i])(si);
	}

	//Otherwise, fork/exec to execute non-built-ins
	return 1;
}


