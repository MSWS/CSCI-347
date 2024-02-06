#include <pwd.h>
#include <stdio.h>

int main(int argc, char* argv) {
	printf("%d", argc);
	struct passwd test = getpwent();
}	
