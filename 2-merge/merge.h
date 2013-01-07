#include "perf.h"

/**
 * The datatype of array elements.
 */
#define TYPE int

extern const char *algorithm_name;

/**
 * a and b are sorted arrays of lengths n and m.
 * No element occurs twice within a and b.
 *
 * These two arrays are merged together. A reference to
 * the resulting array of length n + m is returned and must
 * be freed by the caller.
 */
TYPE *merge(const TYPE *a, int n, const TYPE *b, int m, perf_t *perf, const char *nproc,
        int *nproc_ret);

/**
 * Performs the actual sequential merge within the given memory areas.
 */
void merge_seq(const TYPE *a, int n, const TYPE *b, int m, TYPE *c);

/**
 * The sequential reference implementation.
 */
TYPE *merge_ref(const TYPE *a, int n, const TYPE *b, int m);
