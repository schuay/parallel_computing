#include <stdlib.h>

typedef struct {
    int *counters;
    size_t len;
} perf_t;

perf_t *perf_create(size_t n);
void perf_free(perf_t *perf);

/** Increment counter for thread n. */
void perf_inc(perf_t *perf, size_t n);

/** Print a summary of the performance counters.
 * Return the sum of all counters. */
int perf_summary(perf_t *perf);

