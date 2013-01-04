#include <mpi.h>
#include <stdio.h>
#include <time.h>

#include "bucket.h"
#include "common.h"
#include "csv.h"

#define MASTER (0)

extern const char *algorithm_name;

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    if (argc < 4) {
        fprintf(stderr, "Usage: bench <csv file> <input size> <input upper bound>\n");
        return -1;
    }

    const int size = safe_strtol(argv[2]);
    if (size < 1) {
        fprintf(stderr, "Input size must be greater than 0\n");
        return -1;
    }

    const int upper_bound = safe_strtol(argv[3]);
    if (size < 1) {
        fprintf(stderr, "Input upper bound must be greater than 0\n");
        return -1;
    }

    int processes;
    MPI_Comm_size(MPI_COMM_WORLD, &processes);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /* We need to generate the same array in each process. The master process
     * determines a seed and broadcasts it to all others. */

    int seed = time(NULL);
    MPI_Bcast(&seed, 1, MPI_INT, MASTER, MPI_COMM_WORLD);

    TYPE *a = random_array(size, upper_bound, seed);

    DEBUG("%s. MPI_Comm_size %d, MPI_Comm_rank %d, seed %d\n",
            algorithm_name, processes, rank, seed);

    /* Everything is set up, start sorting and time how long it takes. */

    float start = MPI_Wtime();
    TYPE *c = bucket_sort(a, size, upper_bound, NULL);
    MPI_Barrier(MPI_COMM_WORLD);
    float time = MPI_Wtime() - start;

    free(a);
    free(c);

    /* Only the master process (rank 0) outputs information. */

    if (rank == MASTER) {
        printf("elements: %d; upper bound: %d; time: %f\n\n",
                size, upper_bound, time);

        /* Persist this run in our csv file. */

        FILE *const csvFile = csv_open(argv[1]);
        if (csvFile == NULL) {
            return -1;
        }

        fprintf(csvFile, "%s,%d,%d,%f\n", algorithm_name, processes, size, time);

        csv_close(csvFile);
    }

    MPI_Finalize();

    return 0;
}
