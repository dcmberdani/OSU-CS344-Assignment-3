#ifndef __SmSCMDLINE_H
#define __SmSCMDLINE_H

#include <string.h>

#include "shellinfo.h" // For struct info

/*
 * Name: mainLoop
 * Description: Acts as the main loop for the functionality of the program
 * 	Prints out the prompt, takes in user input, executes commands, and cleans up background processes
 * 	Functionality for individual component is delegated to other functions	
 * 	Loops infinitely until either exited with the BI or terminated in some other way
 * Preconditions: A Struct Information struct already allocated
 * Postconditions: n/a
 */
void mainLoop(struct shellInfo *si);


/*
 * Name: getLineInput 
 * Description: Takes user input in with 'fgets'
 * Preconditions: n/a
 * Postconditions: An allocated string with user input
 */
char* getLineInput();


/*
 * Name: lineIsValid
 * Description: Checks if the first character of a string is an invalid char
 * 	Invalid chars are EOF, a newline, or the null terminator
 * 	Otherwise, the string is valid and passed on
 * Preconditions: A char array
 * Postconditions: Returns whether it is a valid input for the shell
 */
int lineIsValid(char* line);


/*
 * Name: tokenizeLine
 * Description: Tokenizes the passed in string with a small set of delimiters
 * 	Returns an array of separately allocated strings (one for each token)
 * Preconditions: A char array
 * Postconditions: An array of char array of tokenized values
 */
char** tokenizeLine(char* line);


/*
 * Name: countArgs
 * Description: Takes an array of strings
 * 	Then, counts the number of strings in the list
 * Preconditions: An array of strings
 * Postconditions: the number of strings in the array
 */
int countArgs(char** args);


/*
 * Name: expandVars
 * Description: Runs through the an array of strings
 * 	Changes every instance of '$$' into the PID of the shell process
 * Preconditions: An array of strings
 * Postconditions: An array of strings with every instance of '$$' expanded
 */
void expandVars(char** args);

#endif
