#include <stdio.h>
#include <stdlib.h>

int main(char** argv, int argcp) {
	system("echo $USER && grep include activity.c");
	return 0;
}
