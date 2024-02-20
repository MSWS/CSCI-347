#include "builtin.h"

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <fcntl.h>

// Prototypes
static void exitProgram(char** args, int argcp);
static void cd(char** args, int argpcp);
static void pwd(char** args, int argcp);
static void ls(char** args, int argcp);
static void cp(char** args, int argcp);
static void env(char** args, int argcp);
static int internal_cp(const char* from, const char* to);

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
  char* pathBuffer = malloc(PATH_MAX);
	if(pathBuffer == NULL) {
		perror("malloc");
		return;
	}

  if(argcp == 2)
		strcpy(pathBuffer, args[1]);
  else {
		char* homePointer = getenv("HOME");
		if(homePointer == NULL) {
			perror("Error getting home dir");
			return;
		}
		strcpy(pathBuffer, homePointer);
	}
		
  if (chdir(pathBuffer) != 0) perror("chdir");
	free(pathBuffer);
}

static void exitProgram(char** args, int argcp) {
	int exitCode = argcp == 1 ? 0 : atoi(args[1]);
  for (int i = 0; i < argcp + 1; i++) free(args[i]);
  free(args);
  exit(exitCode);
}

// BEGIN LS SECTION

enum { LIST_FANCY = 1 << 0, DONT_LIST_IMPLIED = 1 << 1, LIST_HIDDEN = 1 << 2 };

// Gets the first char to display for a given
// file's flags
static char filetypeletter(int mode) {
  if (S_ISREG(mode)) return '-';
  if (S_ISDIR(mode)) return 'd';
  if (S_ISBLK(mode)) return 'b';
  if (S_ISCHR(mode)) return 'c';
  return '?'; /* Unknown type -- possibly a regular file? */
}

// Given a file name and bitmask, returns
// true if the bitmask indicates the file
// should be ignored based off the bitmask
static bool ignoreFile(const char *name, int bitfield) {
  if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
    if ((bitfield & DONT_LIST_IMPLIED) == DONT_LIST_IMPLIED) return true;

  if (name[0] == '.' && ((bitfield & LIST_HIDDEN) == 0)) return true;
  return false;
}

// Prints out the 9 chars that represent a file's
// permission flags [usr][group][others]
static void printFilePerms(mode_t perms) {
  printf((perms & S_IRUSR) ? "r" : "-");
  printf((perms & S_IWUSR) ? "w" : "-");
  printf((perms & S_IXUSR) ? "x" : "-");
  printf((perms & S_IRGRP) ? "r" : "-");
  printf((perms & S_IWGRP) ? "w" : "-");
  printf((perms & S_IXGRP) ? "x" : "-");
  printf((perms & S_IROTH) ? "r" : "-");
  printf((perms & S_IWOTH) ? "w" : "-");
  printf((perms & S_IXOTH) ? "x" : "-");
}

// Fancy formatting for -l flag
static void lsIndividual(struct dirent *dirEntry, int bitfield) {
  if (ignoreFile(dirEntry->d_name, bitfield)) return;

  struct stat fileStat;
  if (stat(dirEntry->d_name, &fileStat) < 0) {
    perror("Unable to stat file");
    return;
  }

  printf("%c", filetypeletter(fileStat.st_mode));
  printFilePerms(fileStat.st_mode);
  printf(" %lu ", fileStat.st_nlink);

  struct passwd *pwd = getpwuid(fileStat.st_uid);
  if (pwd != NULL)
    printf("%s", pwd->pw_name);
  else
    printf("%d", fileStat.st_uid);

  printf(" ");

  struct group *grp = getgrgid(fileStat.st_gid);
  if (grp != NULL)
    printf("%s", grp->gr_name);
  else
    printf("%d", fileStat.st_gid);

  printf("%8ld ", fileStat.st_size);

  char formattedTimeBuffer[32];
  struct tm *timeInfo = localtime(&(fileStat.st_mtime));
  strftime(formattedTimeBuffer, sizeof(formattedTimeBuffer), "%b %d %H:%M",
           timeInfo);
  printf("%s", formattedTimeBuffer);

  printf(" %s\n", dirEntry->d_name);
}

static void ls(char **args, int argcp) {
  struct dirent *dirEntry;

  int bitfield = 0;
  if (hasFlags(args, argcp, 'l')) bitfield |= LIST_FANCY;
  if (hasFlags(args, argcp, 'A')) {
    bitfield |= DONT_LIST_IMPLIED;
    bitfield |= LIST_HIDDEN;
  }
  if (hasFlags(args, argcp, 'a')) bitfield |= LIST_HIDDEN;

  // Calculate total blocks used in dir
  int total = 0, fileCount = 0;
  struct stat tmpStat;

  DIR *dirPointer = opendir(".");  // open all at present directory

  if (dirPointer == NULL) {
    perror("Could not open current dir");
    return;
  }

  // Collect metadata on both the file count
  // and total file blocks used
  while ((dirEntry = ((struct dirent *)readdir(dirPointer))) != NULL) {
    if (ignoreFile(dirEntry->d_name, bitfield) != 0) continue;

    if (stat(dirEntry->d_name, &tmpStat) < 0) {
      perror("Unable to stat file");
      continue;
      ;
    }

    fileCount++;
    total += tmpStat.st_blocks;
  }

  closedir(dirPointer);
  dirPointer = opendir(".");  // open all at present directory

  if ((bitfield & LIST_FANCY) == 0) {
    // Simple listing is printed out in cols
    // 1. Get the max file name length
    // 2. Get the column width
    // 3. Format accordingly
    char **fileNames = (char **)malloc(fileCount * sizeof(char *));
    if (fileNames == NULL) {
      perror("malloc");
      return;
    }
    int fileIndex = 0, maxNameLength = 0;

    for (int i = 0; i < fileCount; i++) {
      fileNames[i] = (char *)malloc((FILENAME_MAX + 1) * sizeof(char));
      if (fileNames[i] == NULL) {
        perror("malloc");
        closedir(dirPointer);
        for (int j = 0; j < i; j++) free(fileNames[j]);
        free(fileNames);
        return;
      }
    }

    while ((dirEntry = ((struct dirent *)readdir(dirPointer))) != NULL) {
      if (ignoreFile(dirEntry->d_name, bitfield) != 0) continue;
      if (strlen(dirEntry->d_name) > maxNameLength)
        maxNameLength = strlen(dirEntry->d_name);
      strcpy(fileNames[fileIndex++], dirEntry->d_name);
    }

    int columns = 80;  // Default window width
    struct winsize winSize;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &winSize) == -1) {
      perror("ioctl");  // Uh oh, just stick to default then
    } else {
      columns = winSize.ws_col / (maxNameLength + 2);  // + 2 for spacing
    }

    for (int i = 0; i < fileCount; i++) {
      printf("%-*s", maxNameLength + 2, fileNames[i]);
      if ((i + 1) % columns == 0 || i == fileCount - 1) printf("\n");
      free(fileNames[i]);
    }
    free(fileNames);
  } else {
    printf("total %d\n", total / 2);
    while ((dirEntry = ((struct dirent *)readdir(dirPointer))) != NULL)
      lsIndividual(dirEntry, bitfield);
  }

  closedir(dirPointer);
}

// END LS SECTION

// BEGIN CP SECTION

static void cp(char** args, int argcp) {
  if (argcp != 3) {
    printf("Usage: cp [source] [destination]\n");
    return;
  }

  internal_cp(args[1], args[2]);
}

static int internal_cp(const char* from, const char* target) {
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

// END CP SECTION

// BEGIN ENV SECTION

static void env(char** args, int argcp) {
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

// END ENV SECTION
