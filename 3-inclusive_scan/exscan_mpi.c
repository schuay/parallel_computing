#include "exscan.h"

const char *exscan_name = "MPI Exscan";

int exscan(TYPE xi, TYPE *bi, MPI_Comm comm)
{
    return MPI_Exscan(&xi, bi, 1, TYPE_MPI, MPI_SUM, comm);
}
