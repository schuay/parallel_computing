#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#include "merge.h"

#define DEBUG(...) fprintf(stderr, __VA_ARGS__)

/**
 * This function is run on a single thread and handles merging a subsequence.
 * a points to the subsequence of length n. c points to the result array.
 */
static void merge_part(const TYPE *a, int n, const TYPE *b, int m, TYPE *c,
        int iteration, int partitions, perf_t *perf);

/**
 * Performs the actual sequential merge within the given memory areas.
 */
static void merge_seq_private(const TYPE *a, int n, const TYPE *b, int m, TYPE *c);

/**
 * Searches the haystack of size n for needle and returns
 * the closest index it can find.
 */
static int binary_search(TYPE needle, const TYPE *haystack, int n);

TYPE *merge_seq(const TYPE *a, int n, const TYPE *b, int m)
{
    TYPE *c = calloc(n + m, sizeof(TYPE));
    if (c == NULL) {
        return NULL;
    }

    merge_seq_private(a, n, b, m, c);

    return c;
}

static void merge_seq_private(const TYPE *a, int n, const TYPE *b, int m, TYPE *c)
{
    int i = 0;
    int j = 0;
    int k = 0;

    while (i < n && j < m) {
        c[k++] = ((a[i] < b[j]) ? a[i++] : b[j++]);
    }

    while (i < n) c[k++] = a[i++];
    while (j < m) c[k++] = b[j++];
}

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
        merge_part(a, n, b, m, c, i, p, perf);
    }

    return c;
}

static void merge_part(const TYPE *a, int n, const TYPE *b, int m, TYPE *c,
        int iteration, int partitions, perf_t *perf)
{
    /* TODO: If blocks are larger than (m + n) / p, divide further by partitioning
     * the section of b. */

    const int size = n / partitions;
    const int start = iteration * size;
    const int next_start = (iteration + 1) * size;
    const int length = (iteration == partitions - 1) ? n - start : next_start - start;

    DEBUG("iteration %d says hello. from %d, length %d, starts with %d\n",
            iteration, start, length, a[start]);

    int b_start = binary_search(a[start], b, m);

    DEBUG("b partition from %d\n", b_start);
}

static int binary_search(TYPE needle, const TYPE *haystack, int n)
{
    int low = 0;
    int high = n;

    while (low < high) {
        int mid = (low + high) / 2;
        if (needle < haystack[mid]) {
            high = mid;
        } else {
            low = mid + 1;
        }
    }

    return low;
}
