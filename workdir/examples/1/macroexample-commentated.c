#include <stdio.h>

#define describe_macro(number) printf("[DEF] %d is %s", number, number % 2 == 0 ? "even" : "odd")

int counter = 0;
int getNext() {
	return ++counter;
}

void describe_func(int number) {
	printf("[FUNC] %d is %s", number, number % 2 == 0 ? "even" : "odd");
}

int main(int argc, char argv[]) {
	int value = getNext();
	/* describe_func */  printf("[FUNC] %d is %s", value, value  % 2 == 0 ? "even" : "odd");
	printf(" (counter = %d)\n", counter);

	/* describe_macro */ printf("[DEF] %d is %s", getNext(), getNext()  % 2 == 0 ? "even" : "odd");
	printf(" (counter = %d)\n", counter);
}


	
