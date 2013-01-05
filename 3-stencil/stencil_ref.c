#include <mpi.h>
#include <stdio.h>

#include "common.h"
#include "matrix.h"

#define SAVE_INTERVAL (100)

static void usage(void);

/**
 * Saves the matrix into a file, encoding the dimensions, seed
 * and iteration into the filename.
 */
static int save_matrix_at_iteration(const matrix_t *matrix,
        int seed, int iteration);

/**
 * Applies a jacobi iteration to the given matrix, and
 * returns the result as a new matrix. The old matrix is
 * freed.
 */
static matrix_t *jacobi_iteration(matrix_t *matrix);

/**
 * Generates a random n x m matrix with seed s and runs x Jacobi-iterations
 * on it.
 *
 * The matrix is serialized to a file at the first, last, and every 100
 * iterations in-between.
 */
int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    if (argc != 5) {
        usage();
    }

    const int m = safe_strtol(argv[1]);
    const int n = safe_strtol(argv[2]);
    const int s = safe_strtol(argv[3]);
    const int x = safe_strtol(argv[4]);

    if (n < 1 || m < 1 || s < 1 || x < 1) {
        usage();
    }

    matrix_t *matrix = matrix_random(m, n, s);

    double start = MPI_Wtime();
    for (int i = 0; i < x; i++) {
        if (i % SAVE_INTERVAL == 0) {
            save_matrix_at_iteration(matrix, s, i);
        }
        matrix = jacobi_iteration(matrix);
    }
    double end = MPI_Wtime();

    save_matrix_at_iteration(matrix, s, x);
    matrix_free(matrix);

    printf("%dx%d, %d iterations, %f seconds\n",
            m, n, x, end - start);

    MPI_Finalize();

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

static matrix_t *jacobi_iteration(matrix_t *matrix)
{
    int m, n;
    matrix_dims(matrix, &m, &n);

    matrix_t *next = matrix_create(m, n);

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            int count = 0;
            double sum = 0;

            if (i != 0) {
                count++;
                sum += matrix_get(matrix, i - 1, j);
            }
            if (j != 0) {
                count++;
                sum += matrix_get(matrix, i, j - 1);
            }
            if (i != m - 1) {
                count++;
                sum += matrix_get(matrix, i + 1, j);
            }
            if (j != n - 1) {
                count++;
                sum += matrix_get(matrix, i, j + 1);
            }

            matrix_set(next, i, j, sum / count);
        }
    }

    matrix_free(matrix);

    return next;
}
