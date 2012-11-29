#include <check.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * The compile-time type of the array elements. Must have an associative +
 * operator.
 */
#define TYPE int

inline static int is_odd(int n) {
    return n & 0x01;
}

/**
 * Performs an in-place recursive prefix sums calculation on the area located
 * at ptr with len elements of TYPE.
 * Returns 0 on success, < 0 on error.
 */
int prefix_sums(TYPE *x, size_t n) {

    /* All done, terminate. */

    if (n <= 1) {
        return 0;
    }

    /* Create our accumulation array. */

    const size_t m = n / 2;
    TYPE *y = malloc(m * sizeof(TYPE));
    if (y == NULL) {
        return -1;
    }

    /* Accumulate pairs. */

#pragma omp parallel
    {
#pragma omp for
        for (size_t i = 0; i < m; i++) {
            y[i] = x[2 * i] + x[2 * i + 1];
        }
    }

    /* Recurse. */

    if (prefix_sums(y, m) != 0) {
        return -1;
    }

    /* Combine. */

#pragma omp parallel
    {
#pragma omp for
        for (size_t i = 1; i < m; i++) {
            x[2 * i] += y[i - 1];
            x[2 * i + 1] = y[i];
        }
    }
    x[1] = y[0];
    if (is_odd(n)) {
        x[n - 1] += y[m - 1];
    }

    free(y);

    return 0;
}

static TYPE *random_array(size_t n, int seed) {
    TYPE *y = malloc(sizeof(TYPE) * n);
    if (y == NULL) {
        return y;
    }

    srand(seed);
    for (size_t i = 0; i < n; i++) {
        y[i] = (TYPE)rand();
    }

    return y;
}

static TYPE *prefix_sums_ref(const TYPE *x, size_t n) {
    TYPE *y = malloc(sizeof(TYPE) * n);
    if (y == NULL) {
        return y;
    }

    TYPE sum = 0;
    for (size_t i = 0; i < n; i++) {
        sum += x[i];
        y[i] = sum;
    }

    return y;
}

START_TEST(test_recursive_odd) {
    TYPE nrs[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
    size_t len = sizeof(nrs) / sizeof(nrs[0]);

    TYPE *ref = prefix_sums_ref(nrs, len);
    prefix_sums(nrs, len);

    fail_unless(memcmp(nrs, ref, len) == 0,
            "Result not equal to reference implementation");

    free(ref);
}
END_TEST

START_TEST(test_recursive_even) {
    TYPE nrs[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    size_t len = sizeof(nrs) / sizeof(nrs[0]);

    TYPE *ref = prefix_sums_ref(nrs, len);
    prefix_sums(nrs, len);

    fail_unless(memcmp(nrs, ref, len) == 0,
            "Result not equal to reference implementation");

    free(ref);
}
END_TEST

START_TEST(test_recursive_single) {
    TYPE nrs[] = { 0 };
    size_t len = sizeof(nrs) / sizeof(nrs[0]);

    TYPE *ref = prefix_sums_ref(nrs, len);
    prefix_sums(nrs, len);

    fail_unless(memcmp(nrs, ref, len) == 0,
            "Result not equal to reference implementation");

    free(ref);
}
END_TEST

START_TEST(test_recursive_1024) {
    const size_t len = 1024;

    TYPE *nrs = random_array(len, len);
    TYPE *ref = prefix_sums_ref(nrs, len);

    prefix_sums(nrs, len);

    fail_unless(memcmp(nrs, ref, len) == 0,
            "Result not equal to reference implementation");

    free(nrs);
    free(ref);
}
END_TEST

START_TEST(test_recursive_16384) {
    const size_t len = 16384;

    TYPE *nrs = random_array(len, len);
    TYPE *ref = prefix_sums_ref(nrs, len);

    prefix_sums(nrs, len);

    fail_unless(memcmp(nrs, ref, len) == 0,
            "Result not equal to reference implementation");

    free(nrs);
    free(ref);
}
END_TEST

static Suite *recursive_suite(void) {
    Suite *s = suite_create("recursive");

    TCase *tc_core = tcase_create("core");

    tcase_add_test(tc_core, test_recursive_odd);
    tcase_add_test(tc_core, test_recursive_even);
    tcase_add_test(tc_core, test_recursive_single);
    tcase_add_test(tc_core, test_recursive_1024);
    tcase_add_test(tc_core, test_recursive_16384);

    suite_add_tcase(s, tc_core);

    return s;
}

int main(int argc, char **argv) {
    struct opts opts;
    if (parse_args(argc, argv, &opts) != 0) {
        return EXIT_FAILURE;
    }

    printf("omp_get_max_threads() == %d\n", omp_get_max_threads());

    int number_failed;
    Suite *s = recursive_suite();
    SRunner *sr = srunner_create(s);
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
