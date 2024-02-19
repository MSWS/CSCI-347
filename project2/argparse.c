#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * argCount is a helper function that takes in a String and returns the number
 * of "words" in the string assuming that whitespace is the only possible
 * delimeter.
 */
static int argCount(char* line) {
  int count = 0;
  bool inWord = false;

  for (int i = 0; i < strlen(line); i++) {
    if (line[i] == ' ') {
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
char** argparse(char* line, int* argcp) {
  int argc = argCount(line);

  char** arguments = (char**)malloc((argc + 1) * sizeof(char*));
  if (arguments == NULL) {
    perror("malloc");
    return NULL;
  }

  for (int i = 0; i < argc; i++) {
    arguments[i] = (char*)malloc(64 * sizeof(char));
    if (arguments[i] == NULL) {
      perror("malloc");
      for (int j = 0; j < i; j++) free(arguments[j]);
      return NULL;
    }
  }

  *argcp = argc;

  int currentArgIndex = 0;   // Keep track what arg we're modifying
  int currentArgLength = 0;  // Keep track the length of the arg we're modifying
  for (int i = 0; i < strlen(line); i++) {
    if (*(line + i) == ' ') {  // End of current arg
			if(currentArgLength == 0)
				continue;
      *(arguments[currentArgIndex] + currentArgLength) = '\0';
      currentArgLength = 0;
      currentArgIndex++;
      continue;
    }
    (arguments[currentArgIndex][currentArgLength]) = *(line + i);
    currentArgLength++;
  }
  (arguments[currentArgIndex][currentArgLength]) =
      '\0';                                 // Null terminate final arg
  (arguments[currentArgIndex + 1]) = NULL;  // Add NULL terminator for execve

  return arguments;
}
