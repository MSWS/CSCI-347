/* CS 347 -- Mini Shell!
 * Original author Phil Nelson 2000
 */
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "argparse.h"
#include "builtin.h"

#define RED "\x1B[31m"
#define GRN "\x1B[32m"
#define YEL "\x1B[33m"
#define BLU "\x1B[34m"
#define MAG "\x1B[35m"
#define CYN "\x1B[36m"
#define WHT "\x1B[37m"
#define RESET "\x1B[0m"

/* PROTOTYPES */

void processline(char* line);
ssize_t getinput(char** line, size_t* size);

/* main
 * This function is the main entry point to the program.  This is essentially
 * the primary read-eval-print loop of the command interpreter.
 */

int main() {
  char* line = NULL;
  size_t size = 0;

  while (1) {
    size_t length = getinput(&line, &size);
    if (length == -1) {
      fprintf(stderr, "Input error, exiting...\n");
      free(line);
      break;
    }

    processline(line);
    if (line > 0)
    line = NULL;
    size = 0;
  }
  return EXIT_SUCCESS;
}

/* getinput
 * line     A pointer to a char* that points at a buffer of size *size or NULL.
 * size     The size of the buffer *line or 0 if *line is NULL.
 * returns  The length of the string stored in *line.
 *
 * This function prompts the user for input, e.g., %myshell%.  If the input fits
 * in the buffer pointed to by *line, the input is placed in *line.  However, if
 * there is not enough room in *line, *line is freed and a new buffer of
 * adequate space is allocated.  The number of bytes allocated is stored in
 * *size. Hint: There is a standard i/o function that can make getinput easier
 * than it sounds.
 */
ssize_t getinput(char** line, size_t* size) {
  ssize_t len = 0;
  char* buffer = getcwd(NULL, 0);

  printf(BLU "%%myshell%% " CYN "(%s) " RESET, buffer);

  free(buffer);

  len = getline(line, size, stdin);

  if (len == -1) {
    perror("getline");
    return len;
  }

  if (len == 0 || (*line)[len - 1] != '\n') return len;

  (*line)[len - 1] = '\0';
  len--;
  return len;
}

/* processline
 * The parameter line is interpreted as a command name.  This function creates a
 * new process that executes that command.
 * Note the three cases of the switch: fork failed, fork succeeded and this is
 * the child, fork succeeded and this is the parent (see fork(2)).
 * processline only forks when the line is not empty, and the line is not trying
 * to run a built in command
 */
void processline(char* line) {
  if (strlen(line) == 0) return;
  // pid_t cpid;
  // int   status;
  int argCount;
  char** arguments = argparse(line, &argCount);
  free(line); // argparse made a copy of line for handling, so we should free
              // early
  if (builtIn(arguments, argCount) == 0) {
    pid_t id = fork();
    if (id < 0) {
      perror("fork");
      exit(1);
    }
    if (id != 0)
      waitpid(id, NULL, 0);
    else {
      execvp(arguments[0], arguments);
      printf("Invalid command\n");
    }
  }

  for (int i = 0; i < argCount + 1; i++)  // + 1 due to NULL terminator at end
    free(arguments[i]);

  free(arguments);
}
