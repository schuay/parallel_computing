#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#include "prefix.h"

const char *algorithm_name = "reduction";

int prefix_sums(TYPE *x, size_t n, perf_t *perf) {
    TYPE sum = 0;
#pragma omp parallel for schedule(guided) reduction(+:sum)
    for (size_t i = 0; i < n; i++) {
        sum += x[i];
        PERF_INC(perf, omp_get_thread_num());
    }

    return 0;
}
