#ifndef __SmSFUNCTION_H
#define __SmSFUNCTION_H

#define NUM_BUILTINS 3

#include "shellinfo.h" // For struct info


//BUILT INS BELOW
int cdBI(struct shellInfo *si);

int exitBI(struct shellInfo *si);

int statusBI(struct shellInfo *si);

//Checks if it's builtin;
//	If so, run the builting
//	If not, execute a non-built-in
int executeCommand(struct shellInfo *si);

int executeNonBI(struct shellInfo *si);

#endif
