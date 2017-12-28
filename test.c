#include <stdio.h>
#include <stdlib.h>
#include<time.h>

int main(int argc, char **argv)
{
	// printf("%d", argc);
	// printf("%s", argv[0]);
	char *p;
	int x = strtol(argv[1], &p, 10);
	// int x = (int) argv[1];
	printf("%d", x);
}