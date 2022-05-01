// If you are not compiling with the gcc option --std=gnu99, then
// uncomment the following line or you might get a compiler warning
//#define _GNU_SOURCE
/*
 * Dev: Daniel Mendes
 * Date: 5/2/22
 */
//FUNCTION DECLARATIONS BELOW



#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <signal.h>

#include "cmdline.h"
#include "functions.h" // ALREADY INCLUDED IN CMDLINE
//#include "signals.h"


int main(int argc, char *argv[])
{	

	//Set up the struct that contains all information for the shell
	struct shellInfo *si = malloc (sizeof (struct shellInfo));
	//Some necessary initializations
	si->shellPID = getpid();
	si->bgProcessCount = 0;
	si->pIfExited = 0;
	si->pIfSignaled = 0;
	si->status = 1; //Keep running on initial comment
	
	//si->SIGINT_act;
	//si->SIGSTP_act;
	
	//Handles default SIGINT handler stuff
	si->SIGINT_act.sa_handler = SIG_IGN; //Ignore by default
	sigfillset(&si->SIGINT_act.sa_mask); //block all signals while executing handler
	si->SIGINT_act.sa_flags = 0; // no flags set yet
	sigaction(SIGINT, &si->SIGINT_act, NULL); //Redirect functionality to custom struct

	//Handles default SIGSTP handler stuff
	si->SIGSTP_act.sa_handler = handle_SIGSTP; //Run by default
	sigfillset(&si->SIGSTP_act.sa_mask); //block all signals while executing handler
	si->SIGSTP_act.sa_flags = 0; // no flags set yet
	sigaction(SIGTSTP, &si->SIGSTP_act, NULL); //Redirect functionality to custom struct


	// Runs the main shell loop
	mainLoop(si);

	//After finishing the program, free it
	free(si);

	return EXIT_SUCCESS;
}
