#include <stdio.h>

int main() {
	char str[] = "";
	printf("NULL? %d", str[0] == NULL);
	printf("Size: %d", sizeof(str));
}
