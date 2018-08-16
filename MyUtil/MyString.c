#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MyString.h"

char* strsep(char** string, const char* delim) {
    char *start = *string;
    char *p;

    p = (start != NULL) ? strpbrk(start, delim) : NULL;

    if (NULL == p) {
        *string = NULL;
    }
    else {
        *p = '\0';
        *string = p + 1;
    }
    return start;
}