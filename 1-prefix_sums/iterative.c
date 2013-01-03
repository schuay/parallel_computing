#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#include "prefix.h"

const char *algorithm_name = "iterative";

int prefix_sums(TYPE *x, size_t n, perf_t *perf) {
    size_t kk = 2;

    /* Up phase. */

    for (size_t k = 1; k < n; k = kk) {
        kk = k * 2;
#pragma omp parallel for
        for (size_t i = kk - 1; i < n; i += kk) {
            x[i] += x[i - k];
            PERF_INC(perf, omp_get_thread_num());
        }
    }

    /* Down phase. */

    for (size_t k = kk >> 1; k > 1; k = kk) {
        kk = k >> 1;
#pragma omp parallel for
        for (size_t i = k - 1; i < n - kk; i += k) {
            x[i + kk] += x[i];
            PERF_INC(perf, omp_get_thread_num());
        }
    }

    return 0;
}
