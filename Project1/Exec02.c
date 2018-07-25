#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINE 500

char *tok;
char line[MAXLINE];


int main(int argc, char** argv) {

	while (fgets(line, MAXLINE, stdin) != NULL) {
		int i = 0;
		/* first token on each line */
		if ((tok = strtok(line, " \n")) != NULL) {
			puts(tok); 
		}
		/* subsequent tokens */
		while ((tok = strtok(NULL, " \n")) != NULL) {
			puts(tok); 
		}
	}

	return EXIT_SUCCESS;
}