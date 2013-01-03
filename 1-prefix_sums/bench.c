#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <omp.h>
#include <time.h>

#include "common.h"
#include "csv.h"
#include "prefix.h"

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

    const int maxThreads = omp_get_max_threads();

    if (argc < 4) {
        fprintf(stderr, "Usage: bench <csv file> <input size> <num threads> [<num threads> ...]\n");
        return -1;
    }

    FILE *const csvFile = csv_open(argv[1]);
    if (csvFile == NULL) {
        return -1;
    }

    const int len = safe_strtol(argv[2]);
    if (len < 1) {
        fprintf(stderr, "Input size must be positive\n");
        return -1;
    }

    TYPE *nrs = random_array(len, time(NULL));
    if (nrs == NULL) {
        return -1;
    }

    for (int i = 3; i < argc; i++) {
        int threads = safe_strtol(argv[i]);
        if (threads < 1) {
            threads = maxThreads;
        }

        omp_set_num_threads(threads);
        printf("%s. omp_get_max_threads() == %d\n\n", algorithm_name, threads);

        /* Bench the parallel implementation. */

        perf_t *perf = perf_create(threads);
        if (perf == NULL) {
            return -1;
        }

        double start = omp_get_wtime();
        if (prefix_sums(nrs, len, perf) != 0) {
            return -1;
        }
        double par_time = omp_get_wtime() - start;

        printf("elements: %d; par time: %f\n\n",
                len, par_time);
        perf_summary(perf);
        printf("\n");

        perf_free(perf);

        fprintf(csvFile, "%s,%d,%d,%f\n", algorithm_name, threads, len, par_time);
    }

    free(nrs);

    csv_close(csvFile);

    return 0;
}
