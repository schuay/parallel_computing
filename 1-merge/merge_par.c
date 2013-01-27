#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#include "merge.h"

/* #define DEBUG(...) fprintf(stderr, __VA_ARGS__) */
#define DEBUG(...)

const char *algorithm_name = "parallel merge";

/**
 * This function is run on a single thread and handles merging a subsequence.
 * a points to the subsequence of length n. c points to the result array.
 */
static void merge_part(const TYPE *a, int n, const TYPE *b, int m, TYPE *c,
        int iteration, int partitions, int nsplit);

/**
 * Searches the haystack of size n for needle and returns
 * the closest index it can find.
 */
static int binary_search(TYPE needle, const TYPE *haystack, int n);

TYPE *merge(const TYPE *a, int n, const TYPE *b, int m)
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

#pragma omp parallel for schedule(guided)
    for (int i = 0; i < p; i++) {
        merge_part(a, n, b, m, c, i, p, 1);
    }

    return c;
}

static void merge_part(const TYPE *a, int n, const TYPE *b, int m, TYPE *c,
        int iteration, int partitions, int nsplit)
{
    const int size = n / partitions;
    const int start = iteration * size;
    const int next_start = (iteration + 1) * size;
    const int length = (iteration == partitions - 1) ? n - start : next_start - start;

    int b_start = (iteration == 0) ? 0 : binary_search(a[start], b, m);
    int b_length = (iteration == partitions - 1) ?
        m - b_start : binary_search(a[next_start], b, m) - b_start;

    if ((length + b_length) > ((m + n) / partitions) && nsplit <= 1) {
        const int threads = omp_get_max_threads();
        const int p = (threads <= b_length) ? threads : b_length;

#pragma omp parallel for schedule(guided)
        for (int i = 0; i < p; i++) {
            merge_part(b + b_start, b_length,
                    a + start, length,
                    c + start + b_start,
                    i, p, nsplit + 1);
        }

        return;
    }

    DEBUG("iteration %d says hello. from %d, length %d, starts with %d."
          "b partition from %d, length %d\n",
            iteration, start, length, a[start],
            b_start, b_length);

    merge_seq(a + start, length, b + b_start, b_length, c + start + b_start);
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
