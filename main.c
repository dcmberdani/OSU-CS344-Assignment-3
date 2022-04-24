// If you are not compiling with the gcc option --std=gnu99, then
// uncomment the following line or you might get a compiler warning
//#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assignment3.h"


int main(int argc, char *argv[])
{	
	printf("Nothing is going horrible wrong atm.\n");

	// Runs the main shell loop
	mainLoop();

	return EXIT_SUCCESS;
}
