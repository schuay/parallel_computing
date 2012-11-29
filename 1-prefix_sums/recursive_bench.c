#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <omp.h>
#include <time.h>

#include "common.h"
#include "recursive.h"

static int safe_strtol(const char *s) {
    char *endptr;

    errno = 0;
    long val = strtol(s, &endptr, 0);

    /* Check for various possible errors */

    if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
            || (errno != 0 && val == 0)) {
        perror("strtol");
        return -1;
    }

    if (*endptr != '\0')        /* Not necessarily an error... */
        printf("Further characters after number: %s\n", endptr); 

    return val;
}

int main(int argc, char **argv) {

    const int threads = omp_get_max_threads();
    printf("omp_get_max_threads() == %d\n\n", threads);

    for (int i = 1; i < argc; i++) {
        int len = safe_strtol(argv[i]);
        if (len < 1) {
            continue;
        }

        perf_t *perf = perf_create(threads);
        if (perf == NULL) {
            return -1;
        }

        TYPE *nrs = random_array(len, time(NULL));
        if (nrs == NULL) {
            return -1;
        }

        double start = omp_get_wtime();
        if (prefix_sums(nrs, len, perf) != 0) {
            return -1;
        }
        double end = omp_get_wtime();

        int counter = perf_join(perf);
        printf("elements: %d; counter: %d; time: %f\n", len, counter, end - start);

        free(nrs);
        perf_free(perf);
    }

    return 0;
}
