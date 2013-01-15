#ifndef __EXSCAN_H
#define __EXSCAN_H

#include <mpi.h>

extern const char *exscan_name;

int exscan(int xi, int *yi, MPI_Datatype type, MPI_Comm comm);

#endif /* __EXSCAN_H */
