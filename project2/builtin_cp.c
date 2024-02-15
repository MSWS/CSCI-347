#include "builtin_cp.h"

#include <assert.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "builtin.h"

int internal_cp(const char* from, const char* target);

void cp(char** args, int argcp) {
  if (argcp != 3) {
    printf("Usage: cp [source] [destination]\n");
    return;
  }

  internal_cp(args[1], args[2]);
}

int internal_cp(const char* from, const char* target) {
  int fromFD, targetFD;

  char buffer[4096];

  fromFD = open(from, O_RDONLY);
  if (fromFD < 0) return -1;

  targetFD = open(target, O_WRONLY | O_CREAT | O_EXCL, 0666);
  if (targetFD < 0) {
    close(fromFD);
    return -1;
  }

  ssize_t bytesRead;
  while ((bytesRead = read(fromFD, buffer, sizeof(buffer))) > 0) {
    char* outputPtr = buffer;
    ssize_t bytesWritten;

    do {
      bytesWritten = write(targetFD, outputPtr, bytesRead);

      if (bytesWritten >= 0) {
        bytesRead -= bytesWritten;
        outputPtr += bytesWritten;
      } else if (errno != EINTR) {
        close(fromFD);
        close(targetFD);
        return -1;
      }
    } while (bytesWritten > 0);

    if (bytesRead == 0) {
      close(fromFD);
      close(targetFD);
      return 0;
    }

    return -1;  // Something went wrong, we still have bytes remaining!
  }
  return -1;
}
