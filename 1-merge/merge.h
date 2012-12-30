/**
 * The datatype of array elements.
 */
#define TYPE int

/**
 * a and b are sorted arrays of lengths n and m.
 * No element occurs twice within a and b.
 *
 * These two arrays are merged together. A reference to
 * the resulting array of length n + m is returned and must
 * be freed by the caller.
 */
TYPE *merge(const TYPE *a, int n, const TYPE *b, int m);
