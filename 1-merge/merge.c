#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#include "merge.h"

static void merge_part(const TYPE *a, int n, const TYPE *b, int m, TYPE *c, perf_t *perf);

TYPE *merge(const TYPE *a, int n, const TYPE *b, int m, perf_t *perf)
{
    TYPE *c = calloc(n + m, sizeof(TYPE));
    if (c == NULL) {
        return NULL;
    }

    /* Partition a into p partitions (p <= n). Each resulting partition
     * is handled by a thread, which then binary searches b for
     * the appropriate place to start merging.
     */

    const int threads = omp_get_max_threads();
    const int p = (threads <= n) ? threads : n;

#pragma omp parallel for
    for (int i = 0; i < p; i++) {
        const int me = omp_get_thread_num();

        printf("thread %d says hello\n", me);

        const int part_start = i * (n / p);
        const int next_part_start = (i + 1) * (n / p);
        const int part_len = (next_part_start > n) ?
            n - part_start : next_part_start - part_start;

        merge_part(a + part_start, part_len, b, m, c, perf);
    }

    return c;
}

/**
 * This function is run on a single thread and handles merging a subsequence.
 * a points to the subsequence of length n. c points to the point in the result
 * array in which the subsequence should start being merged.
 */
static void merge_part(const TYPE *a, int n, const TYPE *b, int m, TYPE *c, perf_t *perf)
{
    /* TODO: If blocks are larger than (m + n) / p, divide further by partitioning
     * the section of b. */
}
