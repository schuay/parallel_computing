#include <stdlib.h>

#include "perf.h"

perf_t *perf_create(size_t n) {
    perf_t *p = malloc(sizeof(perf_t));
    if (p == NULL) {
        return p;
    }

    p->len = n;
    p->counters = calloc(n, sizeof(int));
    if (p->counters == NULL) {
        free(p);
        return NULL;
    }

    return p;
}

void perf_free(perf_t *perf) {
    free(perf->counters);
    free(perf);
}

void perf_inc(perf_t *perf, size_t n) {
    perf->counters[n]++;
}

int perf_join(perf_t *perf) {
    int sum = 0;
    for (size_t i = 0; i < perf->len; i++) {
        sum += perf->counters[i];
    }
    return sum;
}

