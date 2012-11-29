#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#include "prefix.h"

inline static int is_odd(int n) {
    return n & 0x01;
}

static int prefix_sums_(TYPE *x, size_t n, perf_t *perf, TYPE *y) {

    /* All done, terminate. */

    if (n <= 1) {
        return 0;
    }

    /* Create our accumulation array. */

    const size_t m = n / 2;

    /* Accumulate pairs. */

#pragma omp parallel
    {
#pragma omp for
        for (size_t i = 0; i < m; i++) {
            y[i] = x[2 * i] + x[2 * i + 1];
            perf_inc(perf, omp_get_thread_num());
        }
    }

    /* Recurse. */

    if (prefix_sums_(y, m, perf, y + m) != 0) {
        return -1;
    }

    /* Combine. */

#pragma omp parallel
    {
#pragma omp for
        for (size_t i = 1; i < m; i++) {
            x[2 * i] += y[i - 1];
            x[2 * i + 1] = y[i];
            perf_inc(perf, omp_get_thread_num());
        }
    }
    x[1] = y[0];
    if (is_odd(n)) {
        x[n - 1] += y[m - 1];
        perf_inc(perf, 0);
    }

    return 0;
}

int prefix_sums(TYPE *x, size_t n, perf_t *perf) {
    TYPE *y = malloc(n * sizeof(TYPE));
    if (y == NULL) {
        return -1;
    }

    int ret = prefix_sums_(x, n, perf, y);

    free(y);

    return ret;
}
