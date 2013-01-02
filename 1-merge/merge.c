#include "merge.h"

void merge_seq(const TYPE *a, int n, const TYPE *b, int m, TYPE *c)
{
    int i = 0;
    int j = 0;
    int k = 0;

    while (i < n && j < m) {
        c[k++] = ((a[i] < b[j]) ? a[i++] : b[j++]);
    }

    while (i < n) c[k++] = a[i++];
    while (j < m) c[k++] = b[j++];
}

TYPE *merge_ref(const TYPE *a, int n, const TYPE *b, int m)
{
    TYPE *c = calloc(n + m, sizeof(TYPE));
    if (c == NULL) {
        return NULL;
    }

    merge_seq(a, n, b, m, c);

    return c;
}
