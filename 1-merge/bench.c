#include <omp.h>
#include <stdio.h>
#include <time.h>

#include "common.h"
#include "csv.h"
#include "merge.h"

static TYPE *random_unique_sorted_array(int size);
static int less_than(const void *a, const void *b);

int main(int argc, const char **argv) {
    if (argc < 4) {
        fprintf(stderr, "Usage: bench <csv file> <input size> <num threads>\n");
        return -1;
    }

    const int size = safe_strtol(argv[2]);
    if (size < 1) {
        fprintf(stderr, "Input size must be greater than 0\n");
        return -1;
    }

    int threads = safe_strtol(argv[3]);
    if (threads < 1) {
        threads = omp_get_max_threads();
    }

    omp_set_num_threads(threads);
    omp_set_nested(1);

    int n = size / 2;
    int m = size - n;

    TYPE *a = random_unique_sorted_array(size);
    TYPE *b = a + n;

    qsort(a, n, sizeof(a[0]), less_than);
    qsort(b, m, sizeof(a[0]), less_than);

    FILE *const csvFile = csv_open(argv[1]);
    if (csvFile == NULL) {
        return -1;
    }

    printf("%s. omp_get_max_threads() == %d\n\n", algorithm_name, threads);

    double start = omp_get_wtime();
    TYPE *c = merge(a, n, b, m, NULL);
    double par_time = omp_get_wtime() - start;

    printf("elements: %d; par time: %f\n\n",
            size, par_time);

    fprintf(csvFile, "%s,%d,%d,%f\n", algorithm_name, threads, size, par_time);

    free(a);
    free(c);

    csv_close(csvFile);

    return 0;
}

static int less_than(const void *a, const void *b) {
    TYPE *_a = (TYPE *)a;
    TYPE *_b = (TYPE *)b;
    return (int)(*_a - *_b);
}

static TYPE *random_unique_sorted_array(int size)
{
    TYPE *seq = calloc(size, sizeof(TYPE));

    for (int i = 0; i < size; i++) seq[i] = i;

    srand(time(NULL));

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
