#include <stdlib.h> //Free
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


//'Pushes' all members of the PID list to the lowest indices
//	Run through the list
//		If any processes within the bounds are 0, then run through the list again
//		If any of these processes AREN'T 0, then move them into the slot that IS 0;
//		Repeat for all possible processes, decrementing every time a process is moved
//#include <stdio.h>
void cleanPidList(struct shellInfo *si) {

	//printf("Current processes listed: \n");
	//for (int i = 0; i < si->bgProcessCount + 1; i++) 
		//printf("\tProcess #%d: %d\n", i, si->bgPidList[i]);


	for (int i = 0; i < si->bgProcessCount; i++) {
		if (si->bgPidList[i] != 0) 
			continue;
		


		for (int j = i + 1; j < si->bgProcessCount + 1; j++)
			if (si->bgPidList[j] != 0) {
				si->bgPidList[i] = si->bgPidList[j];
				si->bgPidList[j] = 0;
				//i--; //Since you 'drag back' all values, you have to drag them back forward
				break; //You only need one overwrite
			}

	}
}

