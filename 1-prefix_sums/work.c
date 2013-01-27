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

    if (argc < 4) {
        fprintf(stderr, "Usage: work <csv file> <input size> [<input size> ...]\n");
        return -1;
    }

    FILE *const csvFile = csv_open(argv[1]);
    if (csvFile == NULL) {
        return -1;
    }

    int nThreads = omp_get_max_threads();
    int work;

    for (int i = 2; i < argc; i++) {
        int size = safe_strtol(argv[i]);
        if (size < 1) {
            fprintf(stderr, "Input size must be positive\n");
            return -1;
        }

        TYPE *nrs = random_array(size, time(NULL));
        if (nrs == NULL) {
            return -1;
        }

        printf("%s. omp_get_max_threads() == %d\n", algorithm_name, nThreads);

        /* Bench the parallel implementation. */

        perf_t *perf = perf_create(nThreads);
        if (perf == NULL) {
            return -1;
        }

        if (prefix_sums(nrs, size, perf) != 0) {
            return -1;
        }

        work = perf_summary(perf);
        perf_free(perf);

        printf("elements: %d; work: %d\n\n",
                size, work);

        fprintf(csvFile, "%s,%d,%d,%d\n", algorithm_name, nThreads, size, work);

        free(nrs);
    }

    csv_close(csvFile);

    return 0;
}
