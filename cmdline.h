#ifndef __SmSCMDLINE_H
#define __SmSCMDLINE_H

#include <string.h>

#include "shellinfo.h" // For struct info

void mainLoop();

char* getLineInput();

int lineIsValid(char* line);

char** tokenizeLine(char* line);

int countArgs(char** args);

void expandVars(char** args);

void freeSIMembers(struct shellInfo *si);

#endif
