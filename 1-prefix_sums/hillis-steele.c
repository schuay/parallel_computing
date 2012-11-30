#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#include "prefix.h"

const char *algorithm_name = "hillis-steele";

int prefix_sums(TYPE *x, size_t n, perf_t *perf) {
    TYPE *y = malloc(n * sizeof(TYPE));
    TYPE *t;
    for (size_t k = 1; k < n; k <<= 1) {
#pragma omp parallel
        {
#pragma omp for
            for (size_t i = 0; i < k; i++) {
                y[i] = x[i];
                perf_inc(perf, omp_get_thread_num());
            }
#pragma omp for
            for (size_t i = k; i < n; i++) {
                y[i] = x[i - k] + x[i];
                perf_inc(perf, omp_get_thread_num());
            }
        }

        t = x;
        x = y;
        y = t;
    }

    return 0;
}
