#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>


int main(int argc, char** argv) {
	int a = 1, b = 2, c = 3, d = 4;
	scanf("%d %*d,%d %d", &a, &b, &c);
	printf("%d %d %d %d\n", a, b, c, d);
	return EXIT_SUCCESS;
}