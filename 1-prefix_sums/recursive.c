#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Application options.
 */
struct opts {
    int dummy;
};

/**
 * Parses command line arguments and constructs an options structure
 * accordingly.
 * Returns 0 on success, < 0 on error.
 */
static int parse_args(int argc, char **argv, struct opts *opts);

static void print_array(int *x, size_t n);

/**
 * The compile-time type of the array elements. Must have an associative +
 * operator.
 */
#define TYPE int

/**
 * Performs an in-place prefix sums calculation on the area located at ptr with
 * len elements of TYPE.
 * Returns 0 on success, < 0 on error.
 */
static int prefix_sums(TYPE *ptr, size_t len);

int main(int argc, char **argv) {
    struct opts opts;
    if (parse_args(argc, argv, &opts) != 0) {
        return EXIT_FAILURE;
    }

    printf("omp_get_max_threads() == %d\n", omp_get_max_threads());

    int nrs[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    size_t len = sizeof(nrs) / sizeof(nrs[0]);
    
    if (prefix_sums(nrs, len) != 0) {
        return EXIT_FAILURE;
    }

    print_array(nrs, len);

    return EXIT_SUCCESS;
}

static int parse_args(int argc, char **argv, struct opts *opts) {
    return 0;
}

static void print_array(int *x, size_t n) {
    for (size_t i = 0; i < n; i++) {
        printf("%02d ", x[i]);
    }
    printf("\n");
}

inline static int is_odd(int n) {
    return n & 0x01;
}

/**
 * The recursive prefix sums algorithm.
 */
int prefix_sums(TYPE *x, size_t n) {

    /* All done, terminate. */

    if (n <= 1) {
        return 0;
    }

    /* Create our accumulation array. */

    const size_t m = n / 2;
    TYPE *y = malloc(m * sizeof(TYPE));
    if (y == NULL) {
        return -1;
    }

    /* Accumulate pairs. */

#pragma omp parallel
    {
#pragma omp for
        for (size_t i = 0; i < m; i++) {
            y[i] = x[2 * i] + x[2 * i + 1];
        }
    }

    /* Recurse. */

    if (prefix_sums(y, m) != 0) {
        return -1;
    }

    /* Combine. */

#pragma omp parallel
    {
#pragma omp for
        for (size_t i = 1; i < m; i++) {
            x[2 * i] += y[i - 1];
            x[2 * i + 1] = y[i];
        }
    }
    x[1] = y[0];
    if (is_odd(n)) {
        x[n - 1] += y[m - 1];
    }

    free(y);

    return 0;
}
