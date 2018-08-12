#include <stdio.h>
#include "MyUtil.h"


//#ifdef __cplusplus
//extern "C" {
//#endif

    char* nowTime(char* str) {
        struct timeb tm;

        ftime(&tm);

        return t2s(str, tm);
    }

    char* t2s(char* str, struct timeb tm) {
        char t[24];
        struct tm ti;

        localtime_s(&ti, &tm.time);

        strftime(t, 20, "%Y-%m-%d %H:%M:%S", &ti);
        sprintf_s(str, 24, "%s.%03u", t, tm.millitm);
        return str;
    }

//#ifdef __cplusplus
//}
//#endif
