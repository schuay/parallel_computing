#include <omp.h>
#include <stdio.h>

#include "common.h"
#include "merge.h"

int main(int argc, const char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: bench <input size> <num threads>\n");
        return -1;
    }

    const int size = safe_strtol(argv[1]);
    if (size < 1) {
        fprintf(stderr, "Input size must be greater than 0\n");
        return -1;
    }

    const int threads = safe_strtol(argv[2]);
    if (threads < 1) {
        fprintf(stderr, "Thread count must be greater than 0\n");
        return -1;
    }

    omp_set_num_threads(threads);

    printf("Merging dummy sequences...\n");

    TYPE a[] = {0,2,4,6,8,10,12,14,16,18,20};
    TYPE b[] = {1,3,5,7,9,11,13,15,17,19,21};

    TYPE *c = merge(a, 11, b, 11, NULL);

    for (int i = 0; i < 22; i++) printf("%d ", c[i]);
    printf("\n");

    free(c);

    return 0;
}
