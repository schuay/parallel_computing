#include <mpi.h>
#include <stdio.h>
#include <string.h>

#include "bucket.h"

const char *algorithm_name = "parallel bucket";

static int less_than(const void *a, const void *b);

/* Complexity: O(n/p log (n/p) + p + n + n/p log p).
 * 
 * Without gathering results after conclusion, this reduces to
 * O(n/p log (n/p) + p).
 */

TYPE *bucket_sort(TYPE *xs, int n, int upper_bound, perf_t *perf)
{
    /* Complexity: O(1). */

    int p, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (p > n) {
        fprintf(stderr, "Process count must be less or equal to n\n");
        return NULL;
    }

    /* Our assigned index range is [start, next_start[.
     * There are m elements in this range (except the last process,
     * which may have a shorter range). */

    const int m = n / p;
    const int start = rank * m;
    const int next_start = (rank == p - 1) ? n : (rank + 1) * m;
    const int range_len = upper_bound / p;

    DEBUG("Rank %d, assigned range: ", rank);
    for (int i = start; i < next_start; i++) {
        DEBUG("%d ", xs[i]);
    }
    DEBUG("\n");

    /* Sort our subarray and count the number of elements belonging to each
     * processes range.
     *
     * Complexity: O((n/p) log (n/p)).
     */

    qsort(xs + start, next_start - start, sizeof(TYPE), less_than);

    /* Complexity: O(p). */

    int tx_elems[p];
    memset(tx_elems, 0, sizeof(tx_elems));

    /* Complexity: O(n/p). */

    for (int i = start; i < next_start; i++) {
        int target = xs[i] / range_len;
        if (target >= p) {
            target = p -1;
        }
        tx_elems[target]++;
    }

    /* Complexity: O(p). */

    int tx_locs[p];
    memset(tx_locs, 0, sizeof(tx_locs));

    for (int i = 1; i < p; i++) {
        tx_locs[i] = tx_locs[i - 1] + tx_elems[i - 1];
    }

    /* Exchange the count of elements to transfer with all processes.
     *
     * Complexity: O(p + log p).
     */

    int rx_elems[p];
    memset(rx_elems, 0, sizeof(rx_elems));

    int ret = MPI_Alltoall(tx_elems, 1, MPI_INT, rx_elems, 1, MPI_INT, MPI_COMM_WORLD);
    if (ret != MPI_SUCCESS) {
        return NULL;
    }

    int rx_locs[p];
    memset(rx_locs, 0, sizeof(rx_locs));

    for (int i = 1; i < p; i++) {
        rx_locs[i] = rx_locs[i - 1] + rx_elems[i - 1];
    }

    DEBUG("Rank %d, rx_elems: ", rank);
    for (int i = 0; i < p; i++) {
        DEBUG("%d ", rx_elems[i]);
    }
    DEBUG("\n");

    /* Allocate space for all elements to be received. */

    int elems = 0;
    for (int i = 0; i < p; i++) {
        elems += rx_elems[i];
    }

    TYPE ys[elems];

    /* Actually exchange the elements with all processes.
     *
     * Complexity: O(p + log p)
     */

    ret = MPI_Alltoallv(xs + start, tx_elems, tx_locs, MPI_INT,
        ys, rx_elems, rx_locs, MPI_INT, MPI_COMM_WORLD);
    if (ret != MPI_SUCCESS) {
        return NULL;
    }

    /* Sort the local range.
     *
     * Complexity: O((n/p) log (n/p)).
     */

    qsort(ys, elems, sizeof(TYPE), less_than);

    DEBUG("Rank %d, sorted subarray: ", rank);
    for (int i = 0; i < elems; i++) {
        DEBUG("%d ", ys[i]);
    }
    DEBUG("\n");

    /* Exchange the count of elements to transfer with all processes.
     * This time, we are exchanging the entire owned range with all other processes.
     *
     * Complexity: O(p + n/p).
     */

    memset(rx_elems, 0, sizeof(rx_elems));

    ret = MPI_Allgather(&elems, 1, MPI_INT, rx_elems, 1, MPI_INT, MPI_COMM_WORLD);
    if (ret != MPI_SUCCESS) {
        return NULL;
    }

    memset(rx_locs, 0, sizeof(rx_locs));

    for (int i = 1; i < p; i++) {
        rx_locs[i] = rx_locs[i - 1] + rx_elems[i - 1];
    }

    /* Other code expects the full range to be returned, so gather it all here.
     *
     * Complexity: O(n + log p).
     */

    TYPE *out = malloc(sizeof(TYPE) * n);

    ret = MPI_Allgatherv(ys, elems, MPI_INT, out, rx_elems, rx_locs, MPI_INT, MPI_COMM_WORLD);
    if (ret != MPI_SUCCESS) {
        return NULL;
    }

    DEBUG("Rank %d, sorted array: ", rank);
    for (int i = 0; i < n; i++) {
        DEBUG("%d ", out[i]);
    }
    DEBUG("\n");

    return out;
}

TYPE *random_array(int size, int upper_bound, int seed)
{
    TYPE *seq = calloc(size, sizeof(TYPE));

    for (int i = 0; i < size; i++) seq[i] = i % upper_bound;

    srand(seed);

    TYPE *a = calloc(size, sizeof(TYPE));

    int k = size;
    for (int i = 0; i < size; i++) {
        int j = rand() % k;
        a[i] = seq[j];
        seq[j] = seq[--k];
    }

    free(seq);

    return a;
}

static int less_than(const void *a, const void *b) {
    TYPE *_a = (TYPE *)a;
    TYPE *_b = (TYPE *)b;
    return (int)(*_a - *_b);
}
