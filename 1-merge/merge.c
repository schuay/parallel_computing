#include <stdlib.h>

#include "merge.h"

TYPE *merge(const TYPE *a, int n, const TYPE *b, int m, perf_t *perf)
{
    TYPE *c = calloc(n + m, sizeof(TYPE));
    if (c == NULL) {
        return NULL;
    }

    return c;
}
