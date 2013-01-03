#include "common.h"
#include "prefix.h"

const char *algorithm_name = "sequential reference";

int prefix_sums(TYPE *x, size_t n, __attribute__((unused)) perf_t *perf) {
    TYPE *y = prefix_sums_ref(x, n);
    free(y);

    return 0;
}
