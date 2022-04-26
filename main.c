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

#include "cmdline.h"
//#include "functions.h" ; ALREADY INCLUDED IN CMDLINE


int main(int argc, char *argv[])
{	
	

	// Runs the main shell loop
	mainLoop();

	return EXIT_SUCCESS;
}
