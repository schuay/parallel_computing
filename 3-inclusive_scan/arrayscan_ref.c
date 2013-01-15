#include "arrayscan.h"

static const char *algorithm_name = "sequential reference";

const char *getAlgorithmName(void)
{
    return algorithm_name;
}

TYPE *arrayscan(const TYPE A[], size_t n, __attribute__((unused)) MPI_Comm comm)
{
    return arrayscan_seq(A, n);
}
