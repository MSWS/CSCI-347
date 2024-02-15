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
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <stdbool.h>

#include "builtin_ls.h"

enum {
  LIST_FANCY = 1 << 0,
	DONT_LIST_IMPLIED = 1 << 1,
	LIST_HIDDEN = 1 << 2
};

char filetypeletter(int mode)
{
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

// Given a file name and bitmask, returns
// true if the bitmask indicates the file
// should be ignored based off the bitmask
bool ignoreFile(const char* name, int bitfield) {
  if(strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
		if((bitfield & DONT_LIST_IMPLIED) == DONT_LIST_IMPLIED)
			return true;
	}

  if(name[0] == '.' && ((bitfield & LIST_HIDDEN) == 0))
		return true;

	return false;
}

void printFilePerms(mode_t perms) {
    printf( (perms & S_IRUSR) ? "r" : "-");
    printf( (perms & S_IWUSR) ? "w" : "-");
    printf( (perms & S_IXUSR) ? "x" : "-");
    printf( (perms & S_IRGRP) ? "r" : "-");
    printf( (perms & S_IWGRP) ? "w" : "-");
    printf( (perms & S_IXGRP) ? "x" : "-");
    printf( (perms & S_IROTH) ? "r" : "-");
    printf( (perms & S_IWOTH) ? "w" : "-");
    printf( (perms & S_IXOTH) ? "x" : "-");
}


void lsIndividual(struct dirent *dirEntry, int bitfield) {
  if(ignoreFile(dirEntry->d_name, bitfield))
		return;

	if((bitfield & LIST_FANCY) == 0) {
	  printf("%-8s\n", dirEntry->d_name);
		return;
	}

	struct stat fileStat;
	if(stat(dirEntry->d_name, &fileStat) < 0) {
		perror("Unable to stat file");
		return;
	}

	printf("%c", filetypeletter(fileStat.st_mode));
	printFilePerms(fileStat.st_mode);
  printf(" %lu ", fileStat.st_nlink);

  struct passwd *pwd = getpwuid(fileStat.st_uid);
	if(pwd != NULL)
		printf("%s", pwd->pw_name);
	else
		printf("%d", fileStat.st_uid);

	printf(" ");

  struct group *grp = getgrgid(fileStat.st_gid);
	if(grp != NULL)
		printf("%s", grp->gr_name);
	else
		printf("%d", fileStat.st_gid);

	printf("%8ld ", fileStat.st_size);

	char formattedTimeBuffer[32];
	struct tm* timeInfo = localtime(&(fileStat.st_mtime));
  strftime(formattedTimeBuffer, sizeof(formattedTimeBuffer), "%b %d %H:%M", timeInfo);
	printf("%s", formattedTimeBuffer);

	printf(" %s\n", dirEntry->d_name);
}

void ls(char** args, int argcp) {
	struct dirent *dirEntry;

	int bitfield = 0;
	if(hasFlags(args, argcp, 'l'))
		bitfield |= LIST_FANCY;
	if(hasFlags(args, argcp, 'A')) {
		bitfield |= DONT_LIST_IMPLIED;
		bitfield |= LIST_HIDDEN;
	}
	if(hasFlags(args, argcp, 'a'))
		bitfield |= LIST_HIDDEN;

  // Calculate total blocks used in dir
	int total = 0;
	struct stat tmpStat;

	DIR *dirPointer = opendir("."); // open all at present directory

	if(dirPointer == NULL) {
		perror("Could not open current dir");
		return;
	}
	while((dirEntry = ((struct dirent*) readdir(dirPointer))) != NULL) {
		if(ignoreFile(dirEntry->d_name, bitfield) != 0)
			continue;

	  if(stat(dirEntry->d_name, &tmpStat) < 0) {
	  	perror("Unable to stat file");
	  	return;
	  }

		total += tmpStat.st_blocks;
	}
	closedir(dirPointer);
	dirPointer = opendir("."); // open all at present directory

	printf("total %d\n", total / 2);

	while((dirEntry = ((struct dirent*) readdir(dirPointer))) != NULL)
		lsIndividual(dirEntry, bitfield);

	if((bitfield & LIST_FANCY) != LIST_FANCY)
		printf("\n");
	closedir(dirPointer);
}

