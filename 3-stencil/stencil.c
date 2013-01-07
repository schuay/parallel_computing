#include <mpi.h>
#include <string.h>

#include "matrix.h"
#include "stencil.h"

#define NDIMS (2)
#define DIM_I (0)
#define DIM_J (1)
#define SHIFT_UP (1)
#define SHIFT_DOWN (-1)

enum tags_t {
    TAG_TOP,
    TAG_BOTTOM,
    TAG_LEFT,
    TAG_RIGHT,
};

const char *algorithm_name = "stencil";

static int stencil_iteration(MPI_Comm comm, submatrix_t *submatrix);

void stencil(matrix_t *matrix, int iters, int r, int c, perf_t *perf)
{
    int processes, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (r * c != processes) {
        fprintf(stderr, "r * c != processes, aborting\n");
        return;
    }

    int m, n;
    matrix_dims(matrix, &m, &n);

    if (m % r  != 0 || n % c != 0) {
        fprintf(stderr, "m / r, n / c must be evenly divisible, aborting\n");
        return;
    }

    /* Construct our MPI cartography. */

    MPI_Comm comm;

    int dims[] = { r, c };
    int periods[] = { 0, 0 };
    int ret = MPI_Cart_create(MPI_COMM_WORLD, NDIMS, dims, periods, 1, &comm);
    if (ret != MPI_SUCCESS) {
        return;
    }

    MPI_Comm_size(comm, &processes);
    MPI_Comm_rank(comm, &rank);

    int coords[NDIMS];
    MPI_Cart_coords(comm, rank, NDIMS, coords);

    /* Extract submatrices. */

    const int subm = m / r;
    const int subn = n / c;
    const int subi = coords[DIM_I] * subm;
    const int subj = coords[DIM_J] * subn;

    DEBUG("Rank %d has coords (%d, %d), submatrix %dx%d from (%d, %d)\n",
            rank, coords[DIM_I], coords[DIM_J],
            subm, subn, subi, subj);

    submatrix_t *submatrix = matrix_extract(matrix, subm, subn, subi, subj);

    /* Perform stencil iterations. */

    for (int i = 0; i < iters; i++) {
        ret = stencil_iteration(comm, submatrix);
        if (ret == -1) {
            fprintf(stderr, "GURU MEDITATION\n");
            break;
        }

    }

    /* Gather all resulting submatrices. */

    double submatrices[m * n];
    ret = MPI_Allgather(submatrix->elems, subm * subn, MPI_DOUBLE,
            submatrices, subm * subn, MPI_DOUBLE, comm);
    if (ret == -1) {
        fprintf(stderr, "GURU MEDITATION\n");
    }

    for (int i = 0; i < processes; i++) {
        int peer_coords[NDIMS];
        MPI_Cart_coords(comm, i, NDIMS, peer_coords);

        submatrix_t s;

        s.m = subm;
        s.n = subn;
        s.i = peer_coords[DIM_I] * subm;
        s.j = peer_coords[DIM_J] * subn;

        s.elems = submatrices + i * subm * subn;
        matrix_cram(matrix, &s);
    }


    submatrix_free(submatrix);

    MPI_Comm_free(&comm);
}

static int stencil_iteration(MPI_Comm comm, submatrix_t *submatrix)
{
    const int m = submatrix->m;
    const int n = submatrix->n;

    MPI_Status status;
    int ret, from, to;

    /* From here on, it seems easier to work directly with the
     * double arrays contained in the submatrix. First of all,
     * extract the matrix edges and shift them through our topology.
     *
     * Let's start by shifting the top row.
     */

    double top[n];
    int has_top = 1;
    memcpy(top, submatrix->elems, n * sizeof(double));

    ret = MPI_Cart_shift(comm, DIM_I, SHIFT_DOWN, &from, &to);
    if (ret != MPI_SUCCESS) {
        return -1;
    }

    if (from == MPI_PROC_NULL) {
        has_top = 0;
    }

    ret = MPI_Sendrecv_replace(top, n, MPI_DOUBLE, to, TAG_TOP,
            from, TAG_TOP, comm, &status);
    if (ret != MPI_SUCCESS) {
        return -1;
    }

    /* Now the bottom. */

    double bottom[n];
    int has_bottom = 1;
    memcpy(bottom, submatrix->elems + submatrix_index(submatrix, m - 1, 0), n * sizeof(double));

    ret = MPI_Cart_shift(comm, DIM_I, SHIFT_UP, &from, &to);
    if (ret != MPI_SUCCESS) {
        return -1;
    }

    if (from == MPI_PROC_NULL) {
        has_bottom = 0;
    }

    ret = MPI_Sendrecv_replace(bottom, n, MPI_DOUBLE, to, TAG_BOTTOM,
            from, TAG_BOTTOM, comm, &status);
    if (ret != MPI_SUCCESS) {
        return -1;
    }

    /* Left. */

    double left[m];
    int has_left = 1;

    for (int i = 0; i < m; i++) {
        left[i] = submatrix->elems[submatrix_index(submatrix, i, 0)];
    }

    ret = MPI_Cart_shift(comm, DIM_J, SHIFT_DOWN, &from, &to);
    if (ret != MPI_SUCCESS) {
        return -1;
    }

    if (from == MPI_PROC_NULL) {
        has_left = 0;
    }

    ret = MPI_Sendrecv_replace(left, m, MPI_DOUBLE, to, TAG_LEFT,
            from, TAG_LEFT, comm, &status);
    if (ret != MPI_SUCCESS) {
        return -1;
    }

    /* Right. */

    double right[m];
    int has_right = 1;

    for (int i = 0; i < m; i++) {
        right[i] = submatrix->elems[submatrix_index(submatrix, i, n - 1)];
    }

    ret = MPI_Cart_shift(comm, DIM_J, SHIFT_UP, &from, &to);
    if (ret != MPI_SUCCESS) {
        return -1;
    }

    if (from == MPI_PROC_NULL) {
        has_right = 0;
    }

    ret = MPI_Sendrecv_replace(right, m, MPI_DOUBLE, to, TAG_RIGHT,
            from, TAG_RIGHT, comm, &status);
    if (ret != MPI_SUCCESS) {
        return -1;
    }

    /* We have now gathered all edges from neighboring processes, proceed with
     * a local sequential stencil iteration.
     *
     * Note that the naming can be confusing since edges are named from the sender's
     * perspective. */

    double next[n * m];

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            int count = 0;
            double sum = 0;

            if (i != 0) {
                count++;
                sum += submatrix->elems[submatrix_index(submatrix, i - 1, j)];
            } else if (has_bottom) {
                count++;
                sum += bottom[j];
            }
            if (j != 0) {
                count++;
                sum += submatrix->elems[submatrix_index(submatrix, i, j - 1)];
            } else if (has_right) {
                count++;
                sum += right[i];
            }
            if (i != m - 1) {
                count++;
                sum += submatrix->elems[submatrix_index(submatrix, i + 1, j)];
            } else if (has_top) {
                count++;
                sum += top[j];
            }
            if (j != n - 1) {
                count++;
                sum += submatrix->elems[submatrix_index(submatrix, i, j + 1)];
            } else if (has_left) {
                count++;
                sum += left[i];
            }

            next[submatrix_index(submatrix, i, j)] = sum / count;
        }
    }

    /* Finally, copy over the results to the original submatrix. */

    memcpy(submatrix->elems, next, m * n * sizeof(double));

    return 0;
}
