#ifndef __STENCIL_H
#define __STENCIL_H

#include "matrix.h"
#include "perf.h"

// #define DEBUG(...) printf(__VA_ARGS__)
#define DEBUG(...)

#define MASTER (0)

/**
 * Performs iters Jacobi iterations on matrix, which is distributed
 * into n/c x m/r blocks (c * r = p).
 */
int stencil(matrix_t *matrix, int iters, int r, int c, perf_t *perf);

#endif /* __STENCIL_H */
