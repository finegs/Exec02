#ifndef _MY_UTIL
#define _MY_UTIL

#include <ctime>
#include <sys/timeb.h>

#ifdef __cplusplus
extern "C" {
#endif

    char* nowTime(char* str);
    char* t2s(char* str, struct timeb tm);

#ifdef __cplusplus
}
#endif

#endif

