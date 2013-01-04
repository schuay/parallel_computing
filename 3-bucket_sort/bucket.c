#include <mpi.h>
#include <stdio.h>
#include <string.h>

#include "bucket.h"

const char *algorithm_name = "parallel bucket";

TYPE *bucket_sort(TYPE *xs, int n, int upper_bound, perf_t *perf)
{
    int p, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /* Our assigned index range is [start, next_start[.
     * There are m elements in this range (except the last process,
     * which may have a shorter range). */

    const int m = n / p;
    const int start = rank * m;
    const int next_start = (rank == p - 1) ? n : (rank + 1) * m;

    /* Count the number of occurences of each element in our range. */

    int counts[upper_bound];
    memset(counts, 0, sizeof(counts));

    for (int i = start; i < next_start; i++) {
        counts[xs[i]]++;
    }

    DEBUG("Local range of %d: ", rank);
    for (int i = start; i < next_start; i++) DEBUG("%d ", xs[i]);
    DEBUG("\n");

    /* Sum up these counts over all processes and do a local exscan. */

    int summed_counts[upper_bound];
    int ret = MPI_Allreduce(counts, summed_counts, upper_bound,
            MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    if (ret != MPI_SUCCESS) {
        return NULL;
    }

    int exscan_counts[upper_bound];
    exscan_counts[0] = 0;
    for (int i = 1; i < upper_bound; i++) {
        exscan_counts[i] =  exscan_counts[i - 1] + summed_counts[i - 1];
    }

    /* Determine starting indexes for each bucket in the final sorted
     * array. */

    int starting_indexes[upper_bound];
    memset(starting_indexes, 0, sizeof(starting_indexes));
    ret = MPI_Exscan(counts, starting_indexes, upper_bound,
            MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    if (ret != MPI_SUCCESS) {
        return NULL;
    }

    DEBUG("Combined starting indexes of %d: ", rank);
    for (int i = 0; i < upper_bound; i++) DEBUG("%d ", exscan_counts[i] + starting_indexes[i]);
    DEBUG("\n");

    /* TODO: I'm not sure what exactly steps 5-8 in the algorithm description
     * are supposed to do. The only interpretation I can think of transfers
     * all local elements in another processes 'range' to that process, after
     * which every process can sort its own range.
     *
     * However, this does not result in a complete sorted array, and we still
     * need to do an Allgather to get the entire range.
     *
     * While this works, since we know the target index of each element, we could
     * accomplish this in a simpler way by just moving the elements to
     * their appropriate spot and then doing an Allreduce. We will implement this
     * way until further clarifications.
     */

    TYPE *local_out = calloc(n, sizeof(TYPE));

    for (int i = start; i < next_start; i++) {
        int src = xs[i];
        int dest_index = exscan_counts[src] + starting_indexes[src];
        exscan_counts[src]++;
        local_out[dest_index] = src;
    }

    TYPE *out = calloc(n, sizeof(TYPE));
    ret = MPI_Allreduce(local_out, out, n, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    free(local_out);
    if (ret != MPI_SUCCESS) {
        return NULL;
    }

    DEBUG("Sorted sequence of %d: ", rank);
    for (int i = 0; i < n; i++) DEBUG("%d ", out[i]);
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
