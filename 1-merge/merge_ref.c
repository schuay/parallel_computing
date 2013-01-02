#include "merge.h"

const char *algorithm_name = "sequential reference";

TYPE *merge(const TYPE *a, int n, const TYPE *b, int m, perf_t *perf)
{
    return merge_ref(a, n, b, m);
}
