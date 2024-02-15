#include "builtin.h"
#include <stddef.h>
#include <stdio.h>

int main(char** argv, int argc) {
  	char* params[] = {"pwd"};
		builtIn(params, sizeof(params) / sizeof(char*));
		return 0;
}
