#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h> 
#include <fcntl.h> 
#include <ctype.h>
#include "builtin.h"
#include <string.h>
#include <dirent.h>
#include <stdbool.h>

//Prototypes
static void exitProgram(char** args, int argcp);
static void cd(char** args, int argpcp);
static void pwd(char** args, int argcp);
static void ls(char** args, int argcp);
static void cp(char** args, int argcp);
static void env(char** args, int argcp);

static bool hasFlags(char** args, int argc, char flag) {
	for(int i = 1; i < argc; i++) {
		if(*args[i] != '-')
			continue; // Not a flags argument
		char *index = strchr(args[i] + sizeof(char), flag); // No need to check first char
																						 						// since we know it's a -
		if(index != NULL) // index would be the pointer to the char that flag
											// appears in args. we don't care what specific
											// arg it was found in, just that it was found
			return true;
	}
	return false;
}

/* builtIn
 ** built in checks each built in command the given command, if the given command
 * matches one of the built in commands, that command is called and builtin returns 1.
 *If none of the built in commands match the wanted command, builtin returns 0;
  */
int builtIn(char** args, int argcp)
{
	printf("BuiltIn called with %d args\n", argcp);
	if(argcp <= 0)
		return 0;
	printf("Args[0] = %s\n", args[0]);
	if(strcmp(args[0], "pwd") == 0) {
		pwd(args, argcp);
		return 1;
	}

	if(strcmp(args[0], "cd") == 0) {
		cd(args, argcp);
		return 1;
	}

	if(strcmp(args[0], "ls") == 0) {
	  ls(args, argcp);
		return 1;
	}

	if(strcmp(args[0], "cp") == 0) {
		cp(args, argcp);
		return 1;
	}

	if(strcmp(args[0], "env") == 0) {
		env(args, argcp);
		return 1;
	}

	perror("Unexpected end");
	return 1;
}

static void exitProgram(char** args, int argcp)
{
 //write your code
}

static void pwd(char** args, int argpc)
{
  //write your code

}


static void cd(char** args, int argcp)
{
 //write your code
}

static char filetypeletter(int mode)
{
    char c;

    if (S_ISREG(mode))
        return '-';
    if (S_ISDIR(mode))
        return 'd';
    if (S_ISBLK(mode))
        return 'b';
    if (S_ISCHR(mode))
        return 'c';
    return '?'; /* Unknown type -- possibly a regular file? */
}

enum {
  LIST_FANCY = 1 << 0,
	LIST_ALL = 1 << 1,
	LIST_IMPLIED = 1 << 2
};

static void lsIndividual(struct dirent *dirEntry, int fancy) {
	if(fancy == 0) {
	  printf("%s\n", dirEntry->d_name);
		return;
	}

	struct stat fileStat;
	if(stat(dirEntry->d_name, &fileStat) < 0) {
		perror("Unable to stat file");
		return;
	}
  printf("%c %s\n", filetypeletter(fileStat.st_mode), dirEntry->d_name);
}

static void ls(char** args, int argcp) {
	DIR *dirPointer;
	struct dirent *dirEntry;
	dirPointer = opendir("."); // open all at present directory

	if(dirPointer == NULL) {
		perror("Could not open current dir");
		return;
	}

	int BITFIELD = 0;
	if(hasFlags(args, argcp, 'l'))
		BITFIELD |= LIST_FANCY;
	if(hasFlags(args, argcp, 'a'))
		BITFIELD |= LIST_ALL;
	if(hasFlags(args, argcp, 'A'))
		BITFIELD |= LIST_IMPLIED;
  // int fancy = hasFlags(args, argcp, 'l'), 
	// 		includeAll = hasFlags(args, argcp, 'a'), 
	// 		includeImplied = hasFlags(args, argcp, 'A');

	while((dirEntry = ((struct dirent*) readdir(dirPointer))) != NULL)
		lsIndividual(dirEntry, fancy);

	closedir(dirPointer);
}

static void cp(char** args, int argcp)
{}

static void env(char** args, int argcp)
{}