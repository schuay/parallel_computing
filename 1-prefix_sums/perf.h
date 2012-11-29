typedef struct {
    int *counters;
    size_t len;
} perf_t;

perf_t *perf_create(size_t n);
void perf_free(perf_t *perf);

/** Increment counter for thread n. */
void perf_inc(perf_t *perf, size_t n);

/** Return the sum of all counters. */
int perf_join(perf_t *perf);

