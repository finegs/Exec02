#include <stdlib.h>
#include <string.h>

#include "MySort.h"

/*****************************************************************
*
*
*****************************************************************/
int issort(void *data, int size, int esize, int(*compare)(const void* key1, const void* key2)) {

    char* a = data;
    void *key;

    int i, j;

    if ((key = (char*)malloc(esize)) == NULL) {
        return -1;
    }


    for (j = 1; j < size; j++)
    {
        memcpy(key, &a[j*esize], esize);
        i = j - 1;

        while (i >= 0 && compare(&a[i*esize], key) > 0) {
            memcpy(&a[(i + 1)*esize], &a[i*esize], esize);
            i--;
        }

        memcpy(&a[(i + 1)*esize], key, esize);
    }

    free(key);
    key = NULL;
    return EXIT_SUCCESS;

}

/*************************************************************************************************
*
***************************************************************************************************/
static int compare_int(const void *int1, const void *int2) {
    if (*(const int*)int1 > *(const int*)int2) return 1;
    else if (*(const int*)int1 < *(const int*)int2) return -1;
    else return 0;
}

static int partition(void *data, int esize, int i, int k, int(*compare)(const void *key1, const void *key2)) {
    char    *a = data;
    void    *pval,
        *temp;
    int r[3];

    if ((pval = malloc(esize)) == NULL) return -1;

    if ((temp = malloc(esize)) == NULL) {
        free(pval);
        return -1;
    }

    r[0] = (rand() % (k - i + 1)) + i;
    r[1] = (rand() % (k - i + 1)) + i;
    r[2] = (rand() % (k - i + 1)) + i;

    issort(r, 3, sizeof(int), compare_int);
    memcpy(pval, &a[r[1] * esize], esize);

    i--;
    k++;

    while (1) {
        do { k--; } while (compare(&a[k*esize], pval) > 0);

        do { i++; } while (compare(&a[i*esize], pval));

        if (i >= k) break;
        else {
            memcpy(temp, &a[i*esize], esize);
            memcpy(&a[i*esize], &a[k*esize], esize);
            memcpy(&a[k*esize], temp, esize);
        }
    }

    free(pval);
    free(temp);

    return k;
}

int qksort(void *data, int size, int esize, int i, int k, int(*compare)(const void *key1, const void *key2)) {
    int j;

    while (i < k) {
        if ((j = partition(data, esize, i, k, compare)) < 0) 
            return -1;

        if (qksort(data, size, esize, i, j, compare) < 0)
            return -1;

        i = j + 1;
    }

    return 0;
}