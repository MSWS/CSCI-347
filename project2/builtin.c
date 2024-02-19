#include "builtin.h"

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include<stdlib.h>

// Prototypes
static void exitProgram(char** args, int argcp);
static void cd(char** args, int argpcp);
static void pwd(char** args, int argcp);

bool hasFlags(char** args, int argc, char flag) {
  for (int i = 1; i < argc; i++) {
    if (*args[i] != '-') continue;  // Not a flags argument
    char* index =
        strchr(args[i] + sizeof(char), flag);  // No need to check first char
                                               // since we know it's a -
    if (index != NULL)  // index would be the pointer to the char that flag
                        // appears in args. we don't care what specific
                        // arg it was found in, just that it was found
      return true;
  }
  return false;
}

/* builtIn
 ** built in checks each built in command the given command, if the given
 *command matches one of the built in commands, that command is called and
 *builtin returns 1. If none of the built in commands match the wanted command,
 *builtin returns 0;
 */
int builtIn(char** args, int argcp) {
  if (argcp <= 0) return 0;
  if (strcmp(args[0], "pwd") == 0) {
    pwd(args, argcp);
    return 1;
  }

  if (strcmp(args[0], "cd") == 0) {
    cd(args, argcp);
    return 1;
  }

  if (strcmp(args[0], "ls") == 0) {
    ls(args, argcp);
    return 1;
  }

  if (strcmp(args[0], "cp") == 0) {
    cp(args, argcp);
    return 1;
  }

  if (strcmp(args[0], "env") == 0) {
    env(args, argcp);
    return 1;
  }

  if (strcmp(args[0], "exit") == 0) {
    exitProgram(args, argcp);
  }

  return 0;
}

static void pwd(char** args, int argpc) {
  char buffer[PATH_MAX];
  if (getcwd(buffer, sizeof(buffer)) == NULL) {
    perror("Error getting pwd");
    return;
  }

  printf("%s\n", buffer);
}

static void cd(char** args, int argcp) {
  if (chdir(args[1]) != 0) perror("chdir");
}

static void exitProgram(char** args, int argcp) {
	int exitCode = argcp == 1 ? 0 : atoi(args[1]);
  for (int i = 0; i < argcp + 1; i++) free(args[i]);
  free(args);
  exit(exitCode);
}
