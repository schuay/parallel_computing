#include <mpi.h>
#include <stdio.h>

#include "bucket.h"

const char *algorithm_name = "parallel bucket";

TYPE *bucket_sort(TYPE *xs, int n, int upper_bound, perf_t *perf) {
    return NULL;
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
