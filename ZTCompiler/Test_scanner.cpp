#include <iostream>
#include "Token.h"
#include "Scanner.h"
#include "Test.h"

static void expects(short a, short b) {
	if (!(a == b)) {
		printf("failed\n");
		printf(" %d is expected but got %d\n", a, b);
		exit(1);
	}
	
}

int main() {
	short a = 10;
	short int b = 15;
	expects(25, a + b);

	return 0;
}