#include <stdio.h>
#include <pwd.h>
#include <stdbool.h>

struct passwd *pwd;

int main(int argc, char* argv) {
	setpwent();
	while(true) {
		pwd = getpwent();
		if (pwd == NULL)
			break;
		printf("%s\n", pwd->pw_name);
	}	
}
