#include "shellinfo.h"

//Frees all allocated memory in the shellInfo struct
//	NOT the shellInfo struct itself
void freeSIMembers(struct shellInfo *si)
{
	char* currArg;
	int counter = 0;	
	while (si->args[counter]) {
		currArg = si->args[counter];

		free(currArg);

		counter++;
	}
	
		
	free(si->line);

	free(si->args);
}

