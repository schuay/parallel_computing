#include <stdlib.h>

// #define DEBUG(...) printf(__VA_ARGS__)
#define DEBUG(...)

#define TYPE int

/**
 * xs is an array of n elements taken from the interval
 * [0, upper_bound[.
 *
 * bucket_sort returns a new array containing all elements of
 * xs in ascending order.
 *
 * It is assumed that the process count is >= n.
 */
TYPE *bucket_sort(TYPE *xs, int n, int upper_bound);

/**
 * Returns a random array with the given size and upper bound,
 * constructed using seed.
 */
TYPE *random_array(int size, int upper_bound, int seed);
