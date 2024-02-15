#include <stdbool.h>
#include "builtin_ls.h"
#include "builtin_cp.h"
#include "builtin_env.h"

#ifndef BUILTIN_H
#define BUILTIN_H

/*BuiltIn
* args    args is an array of char*'s that contain a command and the arguements
*that command
*argcp    The count of how many arguments are in args
* returns 1 if it finds a builtin command to run, returns 0 otherwise
*/
int builtIn(char** args, int argcp);

bool hasFlags(char** args, int argc, char flag);

#endif

