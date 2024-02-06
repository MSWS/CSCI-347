#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main() {
	char* sentence = "To C or not to C";
	printf("First letter: %c\n", sentence[0]);

	printf("Fourth letter: %c\n", *(sentence + 3));

	char word[] = "Testing 1 2 3";
	char * mall = (char *) malloc(strlen(word));
	strcpy(mall, word);

	printf("Mall: %s\n", mall);

	return 0;
}
