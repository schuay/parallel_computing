#include "arrayscan.h"

const char *algorithm_name = "sequential reference";

TYPE *arrayscan(const TYPE A[], size_t n, MPI_Comm comm)
{
    return arrayscan_seq(A, n);
}
