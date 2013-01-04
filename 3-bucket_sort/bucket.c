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

    DEBUG("Local range: ");
    for (int i = start; i < next_start; i++) DEBUG("%d ", xs[i]);
    DEBUG("\n");

    DEBUG("Local counts: ");
    for (int i = 0; i < upper_bound; i++) DEBUG("%d ", counts[i]);
    DEBUG("\n");

    /* Sum up these counts over all processes. */

    int summed_counts[upper_bound];
    int ret = MPI_Allreduce(counts, summed_counts, upper_bound,
            MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    if (ret != MPI_SUCCESS) {
        return NULL;
    }

    DEBUG("Summed counts: ");
    for (int i = 0; i < upper_bound; i++) DEBUG("%d ", summed_counts[i]);
    DEBUG("\n");

    TYPE *out = calloc(n, sizeof(TYPE));
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
