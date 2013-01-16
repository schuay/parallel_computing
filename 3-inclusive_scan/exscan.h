#ifndef __EXSCAN_H
#define __EXSCAN_H

#include <mpi.h>

#include "arrayscan.h"

extern const char *exscan_name;

int exscan(TYPE xi, TYPE *bi, MPI_Comm comm);

#endif /* __EXSCAN_H */
