#ifndef __SmSFUNCTION_H
#define __SmSFUNCTION_H

#define NUM_BUILTINS 3

#include "shellinfo.h" // For struct info


//BUILT INS BELOW
/*
 * Name: cdBI
 * Description: Built in function for 'cd'
 * 	Changes the working directory of the process using 'chdir' 
 * 	If no arguments are provided, go to the directory in the HOME env
 * Preconditions: the file path specified as input
 * Postconditions: working directory changed
 */
int cdBI(struct shellInfo *si);


/*
 * Name: exitBI
 * Description: Built in function for 'exit'
 * 	Simply returns 0 to the mainloop, ending the shell
 * Preconditions: The exit command specified
 * Postconditions: The shell stopped running
 */
int exitBI(struct shellInfo *si);


/*
 * Name: statusBI
 * Description: Built in function for 'status'
 * 	Prints out the exit/termination status for the last foreground process
 * 		NOTE: Only prints information for non-builtin functions
 * 	If one hasn't run, then print a message stating so
 * Preconditions: n/a
 * Postconditions: A message printing the status of last foreground process, if any
 */
int statusBI(struct shellInfo *si);


//Checks if it's builtin;
//	If so, run the builting
//	If not, execute a non-built-in
/*
 * Name: executeCommand
 * Description: Determines what kind of command the process is, then executes it
 * 	First, checks to see if the process is to be run in the background or not
 * 	Then, checks to see if there is any form of I/O redirection
 * 		NOTE: I/O redirection ONLY for non-builtins
 * 	Finally, check to see if the command is built in or not, then run it
 * Preconditions: Command line input for arguments
 * Postconditions: An executed command
 */
int executeCommand(struct shellInfo *si);


/*
 * Name: executeNonBI
 * Description: Monster function, handles a number of things;
 * 	First, forks the process into a parent/child process
 * 	In the child process:
 * 		Ignore the SIGTSTP signal 
 * 		If BG, ignore SIGINT and redir I/O to dev/null
 * 		If there is I/O redir, then attempt to open files
 * 			If error, then return exit code 1 but don't close parent
 * 			If none, READ ONLY from input and WRITE ONLY to output
 * 		Then, use execvp to execute the given command
 * 	In the parent process:
 * 		If the child process if background, track its PID in an array	
 *			Then, wait for it to execute without stopping execution
 *		If the child process is not background, pause and wait for it to finish executing
 *			Keep track of flags signaling exit/status and termination/status
 *			If a signal terminated the process, then print out a message saying so
 * Preconditions: A command to execute and necessary parameters
 * Postconditions: That command being executed/tracked
 */
int executeNonBI(struct shellInfo *si);


//Checks if a process is or isn't to have their i/o redirected
/*
 * Name: checkRedirIO
 * Description: Looks through a process to see if there are I/O redirection flags
 * 	If so, then store the input/output directories and flag a variable in SI
 * 	Then, remove both the flag and the directory from the arg list
 * 	Order of I/O flags doesn't matter
 * Preconditions: A command/argument being present
 * Postconditions: The directory for I/O stored, bools flagged
 */
void checkRedirIO(struct shellInfo *si);


//Checks for background
/*
 * Name: isBgProc
 * Description: Checks the final argument in the argument list
 * 	If it is '&', return 1; If not, return 0
 * 	Uses a global variable that interacts with the custom SIGSTP handler
 * Preconditions: An argument list
 * Postconditions: 1 or 0, depending on final argument
 */
int isBgProc(struct shellInfo *si);


/*
 * Name: cleanUpZombies
 * Description: Runs through the list of stored background PIDs
 * 	Uses waitpid to check if any are finished
 * 	If they are, then remove them from the PID array and clean up the array
 * 		Print out a message stating the termination/exit status as well
 * Preconditions: A PID list with some zombies
 * Postconditions: A PID list without any zombies
 */
void cleanUpZombies(struct shellInfo *si);


/*
 * Name: handle_SIGINT
 * Description: Sigint alternate handler
 * 	Doesn't actually do anything, since only foreground child processes access it
 * 	These processes already overwrite sigint handlers so this is kinda pointless
 * 	BUT, it's a part of my un-ignoring sigint code so if it aint broke dont fix it
 * Preconditions: n/a
 * Postconditions: sigint signal re-handled
 */
void handle_SIGINT();


/*
 * Name: handle_SIGSTP
 * Description: SIGTSTP custom handler
 * 	Toggles a global variable that decides whether the shell is in foreground-only mode
 * 		This global variable is also used in the 'isBgProc' function
 * 	Also prints a handy message to the user about which mode they are in
 * Preconditions: n/a
 * Postconditions: foreground only mode toggled on/off
 */
void handle_SIGSTP();

#endif
