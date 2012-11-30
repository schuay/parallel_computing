#include "perf.h"

/**
 * The compile-time type of the array elements. Must have an associative +
 * operator.
 */
#define TYPE int

/**
 * Performs an in-place recursive prefix sums calculation on the area located
 * at ptr with len elements of TYPE.
 * Returns 0 on success, < 0 on error.
 */
int prefix_sums(TYPE *x, size_t n, perf_t *perf);

extern const char *algorithm_name;
