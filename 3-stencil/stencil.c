#include <mpi.h>

#include "matrix.h"
#include "stencil.h"

#define NDIMS (2)

const char *algorithm_name = "stencil";

void stencil(matrix_t *matrix, int iters, int r, int c, perf_t *perf)
{
    int processes, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (r * c != processes) {
        fprintf(stderr, "r * c != processes, aborting\n");
        return;
    }

    MPI_Comm comm;

    int dims[] = { r, c };
    int periods[] = { 0, 0};
    int ret = MPI_Cart_create(MPI_COMM_WORLD, NDIMS, dims, periods, 1, &comm);
    if (ret != MPI_SUCCESS) {
        return;
    }

    MPI_Comm_rank(comm, &rank);

    int coords[NDIMS];
    MPI_Cart_coords(comm, rank, NDIMS, coords);

    DEBUG("Rank %d has coords (%d, %d)\n", rank, coords[0], coords[1]);

    MPI_Comm_free(&comm);
}
