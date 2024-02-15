#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "argparse.h"
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdbool.h>

#define FALSE (0)
#define TRUE  (1)

/*
* argCount is a helper function that takes in a String and returns the number of "words" in the string assuming that whitespace is the only possible delimeter.
*/
static int argCount(char*line) {
	int count = 0;
	bool inWord = false;

	for(int i = 0; i < strlen(line); i++) {
		if(line[i] == ' ') {
		  inWord = false;
		} else if (!inWord) {
			count++;
			inWord = true;
		}
	}
	
	return count;
}



/*
*
* Argparse takes in a String and returns an array of strings from the input.
* The arguments in the String are broken up by whitespaces in the string.
* The count of how many arguments there are is saved in the argcp pointer
*/
char** argparse(char* line, int* argcp)
{
	int argc = argCount(line);

	char** arguments = (char**) malloc(argc * sizeof(char *));

	for(int i = 0; i < sizeof(arguments); i++) {
	    arguments[i] = (char*) malloc(64 * sizeof(char));
	}

	if(arguments == NULL) {
		perror("malloc");
		return NULL;
	}

  *argcp = argc;

	int currentArgIndex = 0;
	int currentArgLength = 0;
	for(int i = 0; i < strlen(line); i++){
	  if(*(line + i) == ' ') {
			*(arguments[currentArgIndex]+currentArgLength) = '\0';
			currentArgLength = 0;
			currentArgIndex++;
			continue;
		}
		(arguments[currentArgIndex][currentArgLength]) = *(line + i);
		currentArgLength++;
	}
	(arguments[currentArgIndex][currentArgLength]) = '\0';
	(arguments[currentArgIndex + 1]) = NULL;

	return arguments;
}

