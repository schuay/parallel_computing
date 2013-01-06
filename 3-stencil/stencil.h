#ifndef __STENCIL_H
#define __STENCIL_H

#include "matrix.h"
#include "perf.h"

/**
 * Performs iters Jacobi iterations on matrix, which is distributed
 * into n/c x m/r blocks (c * r = p).
 */
void stencil(matrix_t *matrix, int iters, int r, int c, perf_t *perf);

#endif /* __STENCIL_H */
