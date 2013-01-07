#include "merge.h"

#include <cilk.h>

extern void EXPORT(merge_impl) (CilkContext *context, const TYPE *a, int n,
        const TYPE *b, int m, TYPE *c, perf_t *perf, int *nproc);

void merge_seq(const TYPE *a, int n, const TYPE *b, int m, TYPE *c)
{
    int i = 0;
    int j = 0;
    int k = 0;

    while (i < n && j < m) {
        c[k++] = ((a[i] < b[j]) ? a[i++] : b[j++]);
    }

    while (i < n) c[k++] = a[i++];
    while (j < m) c[k++] = b[j++];
}

TYPE *merge_ref(const TYPE *a, int n, const TYPE *b, int m)
{
    TYPE *c = calloc(n + m, sizeof(TYPE));
    if (c == NULL) {
        return NULL;
    }

    merge_seq(a, n, b, m, c);

    return c;
}

TYPE *merge(const TYPE *a, int n, const TYPE *b, int m, perf_t *perf, const char *nproc,
        int *nproc_ret) {
    const char *argv[] = {algorithm_name, "--nproc", nproc, 0};
    int argc = 3;
    TYPE *c = calloc(n + m, sizeof(TYPE));
    if (c == NULL) {
        return NULL;
    }

    CilkContext *context = Cilk_init(&argc, (char **) argv);
    EXPORT(merge_impl) (context, a, n, b, m, c, perf, nproc_ret);
    Cilk_terminate(context);

    return c;
}
