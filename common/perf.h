#ifndef __PERF_H
#define __PERF_H

#include <stdlib.h>

#define PERF_INC(perf, n) perf_inc(perf, n);
#ifdef NOPERF
    #undef PERF_INC
    #define PERF_INC(perf, n)
#endif /* NOPERF */

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

#endif /* __PERF_H */
