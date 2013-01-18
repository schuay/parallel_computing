#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#include "prefix.h"

const char *algorithm_name = "reduction";

int prefix_sums(TYPE *x, size_t n, perf_t *perf) {
    int sum = 0;
#pragma omp parallel for schedule(runtime) reduction(+:sum)
    for (int i = 0; i < n; i++) {
        sum += x[i];
    }

    return 0;
}
