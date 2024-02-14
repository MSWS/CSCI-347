#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "argparse.h"
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#define FALSE (0)
#define TRUE  (1)

/*
* argCount is a helper function that takes in a String and returns the number of "words" in the string assuming that whitespace is the only possible delimeter.
*/
static int argCount(char*line)
{
  int count = 0;
  for(int i = 0; i < strlen(line); i++)
    if(*(line + i) == ' ')
      count++;
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
  // Worst case scenario of the resulting buffer would be
  // twice the strlen(line) in the case of every other line being
  // a whitespace, ie: a b c d e f
  // would become ['a', '\0'], ['b', '\0'], ['c', '\0'], ['d', '\0'], ['e', '\0'] ... [6x 2 char arrays = 12 chars alloted total]
  // and ab cd ef gh
  // would become ['a', 'b', '\0'], ['c', 'd', '\0'], ... [3x 3 char arrays = 9 chars alloted total]
  char* currentArgBuffer = (char *) malloc(sizeof(char) * strlen(line));
  int currentArgBufferLength = 0;
  char** resultBuffer = (char **) malloc(sizeof(char) * strlen(line));
  int resultIndex = 0;

  int currentArgIndex = 0;
  for(int i = 0; i < strlen(line); i++) {
    if(*(line + i) == ' ') {
      currentArgBuffer[currentArgBufferLength++] = '\0';
      strcpy(resultBuffer[resultIndex++], currentArgBuffer); 
    }
    currentArgBuffer[currentArgBufferLength++] = *(line + i);
  }
}

