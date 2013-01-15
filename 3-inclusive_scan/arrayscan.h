#ifndef __ARRAYSCAN_H
#define __ARRAYSCAN_H

#include <mpi.h>

/**
 * The datatype of array elements.
 */
#define TYPE int
#define TYPE_MPI MPI_INT

const char *getAlgorithmName(void);

TYPE *arrayscan(const TYPE A[], size_t n, MPI_Comm comm);

TYPE *arrayscan_seq(const TYPE A[], size_t n);

TYPE *random_array(size_t n, int seed);

#endif /* __ARRAYSCAN_H */
