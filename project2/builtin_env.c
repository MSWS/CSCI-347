#include "builtin_env.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void env(char** args, int argcp) {
  if (argcp == 1) {
    printf("Usage: env [key](=VALUE)\n");
    return;
  }

  char* equalsChar = strchr(args[1], '=');
  if (equalsChar == NULL) {
    char* value = getenv(args[1]);
    if (value == NULL) {
      printf("%s is not set\n", args[1]);
      return;
    }
    printf("%s=%s\n", args[1], value);
    return;
  }

  int keyLength = equalsChar - args[1];
  char* envKey = (char*) malloc((keyLength + 1) * sizeof(char));

  if(envKey == NULL) {
    perror("malloc");
    return;
  }

  for(int i = 0; i < keyLength; i++)
    envKey[i] = args[1][i];
  envKey[keyLength] = '\0';

  int valueLength = strlen(args[1]) - keyLength;
  for(int i = 2; i < argcp; i++)
    valueLength += strlen(args[i]) + 1;

  char* envValue = (char*) malloc((valueLength + 1) * sizeof(char));

  if(envValue == NULL) {
    free(envKey);
    perror("malloc");
    return;
  }

  strcpy(envValue, equalsChar + 1);

  for(int i = 2; i < argcp; i++) { // Support for spaces in env vars
    strcat(envValue, " ");
    strcat(envValue, args[i]);
  }

  setenv(envKey, envValue, 1);

  printf("%s=%s\n", envKey, envValue);
  free(envValue);
  free(envKey);
}
