#include "builtin_env.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void env(char** args, int argcp) {
  if (argcp == 1) {
    printf("Missing param\n");
    return;
  }

  char* equalsChar = strchr(args[1], '=');
  if (equalsChar == NULL) {
    printf("%s=%s\n", args[1], getenv(args[1]));
    return;
  }

  int combinedStringLength = equalsChar - args[1];
  for (int i = 2; i < argcp; i++) {
    combinedStringLength += strlen(args[i]) + 1;
  }

  char* combinedValue = (char*)calloc(combinedStringLength + 1, sizeof(char));
  for (int i = 1; i < argcp; i++) {
    strcat(combinedValue, args[i]);
    strcat(combinedValue, " ");
  }

  printf("%s\n", combinedValue);
  putenv(combinedValue);

  free(combinedValue);
}
