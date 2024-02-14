#include "builtin.h"
#include <stddef.h>
#include <stdio.h>

int main() {
		char* params[] = {"ls", "-l"};
		builtIn(params, sizeof(params) / sizeof(char*));
		return 0;
}
