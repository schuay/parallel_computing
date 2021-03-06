#include <mpi.h>
#include <stdio.h>
#include <time.h>

#include "common.h"
#include "csv.h"
#include "matrix.h"
#include "stencil.h"

extern const char *algorithm_name;

int main(int argc, char **argv)
{
    int ret = 0;
    matrix_t *matrix = NULL;

    MPI_Init(&argc, &argv);

    if (argc != 8) {
        fprintf(stderr, "Usage: bench <csv file> <m> <n> <r> <c> <seed> <iterations>\n");
        ret = -1;
        goto out;
    }

    const char *filename = argv[1];
    const int m = safe_strtol(argv[2]);
    const int n = safe_strtol(argv[3]);
    const int r = safe_strtol(argv[4]);
    const int c = safe_strtol(argv[5]);
    const int seed = safe_strtol(argv[6]);
    const int iterations = safe_strtol(argv[7]);

    if (m < 1 || n < 1 || r < 1 || c < 1 || seed < 1 || iterations < 1) {
        fprintf(stderr, "All numeric arguments must be greater than 0\n");
        ret = -1;
        goto out;
    }

    int processes, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    DEBUG("%s. MPI_Comm_size %d, MPI_Comm_rank %d, seed %d\n",
            algorithm_name, processes, rank, seed);

    matrix = matrix_random(m, n, seed);

    /* Everything is set up, start working and time how long it takes. */

    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();
    if (stencil(matrix, iterations, r, c) != 0) {
        /* Don't fail our benchmarks. */
        goto out;
    }
    double end = MPI_Wtime();

    double localElapsed = end - start;
    double totalElapsed;
    MPI_Reduce(&localElapsed, &totalElapsed, 1, MPI_DOUBLE, MPI_MAX, MASTER, MPI_COMM_WORLD);

    /* Only the master process (rank 0) outputs information. */

    if (rank == MASTER) {
        printf("%dx%d, rxc: %dx%d, %d iterations, %d processes, time: %f\n",
                m, n, r, c, iterations, processes, totalElapsed);

        /* Persist this run in our csv file. */

        FILE *const csvFile = csv_open(filename);
        if (csvFile == NULL) {
            ret = -1;
            goto out;
        }

        fprintf(csvFile, "%s-%dx%d,%d,%d,%f\n", algorithm_name, m / r, n / c,
                processes, n * m * iterations, totalElapsed);

        csv_close(csvFile);
    }

out:
    matrix_free(matrix);

    MPI_Finalize();

    return ret;
}
