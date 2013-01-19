#include <mpi.h>
#include <stdio.h>
#include <time.h>

#include "bucket.h"
#include "common.h"
#include "csv.h"

#define MASTER (0)

extern const char *algorithm_name;

static int less_than(const void *a, const void *b)
{
    return ((*(const int *)a) - (*(const int *)b));
}

int main(int argc, char **argv)
{
    int ret = 0;

    MPI_Init(&argc, &argv);

    if (argc < 4) {
        fprintf(stderr, "Usage: bench <csv file> <input size> <input upper bound>\n");
        ret = -1;
        goto out;
    }

    const char *filename = argv[1];
    const int size = safe_strtol(argv[2]);
    const int upper_bound = safe_strtol(argv[3]);

    if (size < 1 || upper_bound < 1) {
        fprintf(stderr, "Input size and upper bounds must be greater than 0\n");
        ret = -1;
        goto out;
    }

    int seed = time(NULL);
    TYPE *a = random_array(size, upper_bound, seed);

    /* Everything is set up, start sorting and time how long it takes. */

    double start = MPI_Wtime();
    qsort(a, size, sizeof(TYPE), less_than);
    double end = MPI_Wtime();

    free(a);

    /* Only the master process (rank 0) outputs information. */

    printf("sequential. elements: %d; upper bound: %d; time: %f\n",
            size, upper_bound, end - start);

    /* Persist this run in our csv file. */

    FILE *const csvFile = csv_open(filename);
    if (csvFile != NULL) {
        fprintf(csvFile, "%s,%d,%d,%f\n", "sequential reference", 1, size, end - start);
        csv_close(csvFile);
    }

out:
    MPI_Finalize();

    return ret;
}
