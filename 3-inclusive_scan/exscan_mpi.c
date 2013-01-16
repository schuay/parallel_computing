#include "exscan.h"

const char *exscan_name = "MPI Exscan";

int exscan(TYPE xi, TYPE *bi, MPI_Comm comm)
{
    int rank;
    MPI_Comm_rank(comm, &rank);

    int ret = MPI_Exscan(&xi, bi, 1, TYPE_MPI, MPI_SUM, comm);

    if (rank == 0) {
        *bi = 0;
    }

    return ret;
}
