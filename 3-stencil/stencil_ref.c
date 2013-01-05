#include <stdio.h>

#include "common.h"
#include "matrix.h"

static void usage(void);

/**
 * Generates a random n x m matrix with seed s and runs x Jacobi-iterations
 * on it.
 *
 * The matrix is serialized to a file at the first, last, and every 100
 * iterations in-between.
 */
int main(int argc, char **argv)
{
    if (argc != 5) {
        usage();
    }

    const int m = safe_strtol(argv[1]);
    const int n = safe_strtol(argv[2]);
    const int s = safe_strtol(argv[3]);
    const int x = safe_strtol(argv[4]);

    matrix_t *matrix = matrix_random(m, n, s);

    matrix_save(matrix, stdout);

    matrix_free(matrix);

    return 0;
}

static void usage(void)
{
    fprintf(stderr, "Usage: stencil_ref <m> <n> <seed> <iterations>\n");
    exit(0);
}
