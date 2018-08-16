#ifndef _MY_SORT
#define _MY_SORT

#ifdef __cplusplus
extern "C" {
#endif

int issort(void *data, int size, int esize, int(*compare)(const void* key1, const void* key2));
int qksort(void *data, int size, int esize, int i, int k, int(*compare)(const void *key1, const void *key2));
#ifdef __cplusplus
}
#endif

#endif