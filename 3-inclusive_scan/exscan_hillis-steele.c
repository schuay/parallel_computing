#include "exscan.h"

const char *exscan_name = "hillis-steele";

int exscan(TYPE xi, TYPE *bi, MPI_Comm comm)
{
    int processes, rank;
    int ret = -1;

    MPI_Comm_size(comm, &processes);
    MPI_Comm_rank(comm, &rank);

    TYPE tmp, yi;

    MPI_Status status;

    TYPE orig = xi;
    for (int k = 1; k < processes; k <<= 1) {
        if (rank < k) {
            yi = xi;
            ret = MPI_Send(&xi, 1, TYPE_MPI, rank + k, k, comm);
        } else {
            if (rank >= processes - k) {
                ret = MPI_Recv(&yi, 1, TYPE_MPI, rank - k, k, comm, &status);
            } else {
                ret = MPI_Sendrecv(&xi, 1, TYPE_MPI, rank + k, k,
                        &yi, 1, TYPE_MPI, rank - k, k,
                        comm, &status);
            }

            yi += xi;
        }

        if (ret != MPI_SUCCESS) {
            return ret;
        }

        tmp = xi;
        xi = yi;
        yi = tmp;
    }

    // TODO: this is not allowed, we may only assume associativity
    *bi = xi - orig;

    return 0;
}
