#include <stdio.h>

#include "common.h"
#include "matrix.h"

static void usage(void);
static int save_matrix_at_iteration(const matrix_t *matrix,
        int seed, int iteration);

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

    save_matrix_at_iteration(matrix, s, 0);

    matrix_free(matrix);

    return 0;
}

static void usage(void)
{
    fprintf(stderr, "Usage: stencil_ref <m> <n> <seed> <iterations>\n");
    exit(0);
}

static int save_matrix_at_iteration(const matrix_t *matrix,
        int seed, int iteration)
{
    int ret = 0;

    int m, n;
    matrix_dims(matrix, &m, &n);

    char *filename;
    int bytes = asprintf(&filename, "matrix_%dx%d_%d_%d",
            m, n, seed, iteration);

    if (bytes == -1) {
        return -1;
    }

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        return -1;
    }

    if (matrix_save(matrix, file) != 0) {
        ret = -1;
    }

    fclose(file);

    return ret;
}
