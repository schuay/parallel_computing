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

#pragma omp parallel
    {
#pragma omp for
        for (int i = 0; i < 32; i++) {
            usleep(1000);
            printf("Embarassingly parallel %02d on thread %02d\n",
                   i,
                   omp_get_thread_num());
        }
    }

    return EXIT_SUCCESS;
}

static int parse_args(int argc, char **argv, struct opts *opts) {
    return 0;
}

static int prefix_sums(TYPE *ptr, size_t len) {
    return 0;
}
