#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stddef.h>
#include <sys/stat.h>

int main(char** argv, int argc) {
  struct stat fileStat;

  if(stat("foo.txt", &fileStat) < 0) {
    perror("Unable to stat file");
    return 1;
  }

  struct passwd *pwd = getpwuid(fileStat.st_uid);
  if(pwd != NULL)
      printf("pwd: %s\n", pwd->pw_name);
  return 0;
}
         
