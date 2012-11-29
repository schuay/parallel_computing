#include <stdlib.h>

#include "common.h"

TYPE *random_array(size_t n, int seed) {
    TYPE *y = malloc(sizeof(TYPE) * n);
    if (y == NULL) {
        return y;
    }

    srand(seed);
    for (size_t i = 0; i < n; i++) {
        y[i] = (TYPE)rand();
    }

    return y;
}

TYPE *prefix_sums_ref(const TYPE *x, size_t n) {
    TYPE *y = malloc(sizeof(TYPE) * n);
    if (y == NULL) {
        return y;
    }

    TYPE sum = 0;
    for (size_t i = 0; i < n; i++) {
        sum += x[i];
        y[i] = sum;
    }

    return y;
}
