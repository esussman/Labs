
#include <stdlib.h>
#include <stdio.h>
#include "fib.h"

int main(int argc, char ** argv)
{
	int number = atoi(argv[1]);
	printf("%d\n", fib(number));
	return 0;
}
