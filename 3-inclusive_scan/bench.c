#include <mpi.h>
#include <stdio.h>
#include <time.h>

#include "arrayscan.h"
#include "common.h"
#include "csv.h"

#define MASTER (0)

/* #define DEBUG(...) fprintf(stderr, __VA_ARGS__) */
#define DEBUG(...)

int main(int argc, char **argv)
{
    int ret = 0;

    MPI_Init(&argc, &argv);

    if (argc != 4) {
        fprintf(stderr, "Usage: bench <csv file> <input size> <seed>\n");
        ret = -1;
        goto out;
    }

    const char *filename = argv[1];
    const int size = safe_strtol(argv[2]);
    const int seed = safe_strtol(argv[3]);

    if (size < 1) {
        fprintf(stderr, "All numeric arguments must be greater than 0\n");
        ret = -1;
        goto out;
    }

    int processes, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    DEBUG("%s. MPI_Comm_size %d, MPI_Comm_rank %d, input size %d\n",
            getAlgorithmName(), processes, rank, size);

    if (rank == MASTER) {
        printf("%s. nproc == %d\n", getAlgorithmName(), processes);
    }

    TYPE *nrs = random_array(size, seed);
    if (nrs == NULL) {
        ret = -1;
        goto out;
    }

    /* Everything is set up, start working and time how long it takes. */

    double start = MPI_Wtime();
    TYPE *prefix_sums = arrayscan(nrs, size, MPI_COMM_WORLD);
    double end = MPI_Wtime();

    free(nrs);
    if (prefix_sums != nrs) {
    	free(prefix_sums);
    }

    /* Only the master process (rank 0) outputs information. */

    if (rank == MASTER) {
        printf("time: %f\n\n", end - start);

        /* Persist this run in our csv file. */

        FILE *const csvFile = csv_open(filename);
        if (csvFile == NULL) {
            ret = -1;
            goto out;
        }

        fprintf(csvFile, "%s,%d,%d,%f\n", getAlgorithmName(),
                processes, size, end - start);

        csv_close(csvFile);
    }

out:
    MPI_Finalize();

    return ret;
}
