#include "exscan.h"

const char *exscan_name = "MPI Exscan";

int exscan(int xi, int *yi, MPI_Datatype type, MPI_Comm comm)
{
    return MPI_Exscan(&xi, yi, 1, type, MPI_SUM, comm);
}
