#include <check.h>

#include "common.h"
#include "recursive.h"

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
    prefix_sums(nrs, len, NULL);

    fail_unless(memcmp(nrs, ref, len) == 0,
            "Result not equal to reference implementation");

    free(ref);
}
END_TEST

START_TEST(test_recursive_even) {
    TYPE nrs[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    size_t len = sizeof(nrs) / sizeof(nrs[0]);

    TYPE *ref = prefix_sums_ref(nrs, len);
    prefix_sums(nrs, len, NULL);

    fail_unless(memcmp(nrs, ref, len) == 0,
            "Result not equal to reference implementation");

    free(ref);
}
END_TEST

START_TEST(test_recursive_single) {
    TYPE nrs[] = { 0 };
    size_t len = sizeof(nrs) / sizeof(nrs[0]);

    TYPE *ref = prefix_sums_ref(nrs, len);
    prefix_sums(nrs, len, NULL);

    fail_unless(memcmp(nrs, ref, len) == 0,
            "Result not equal to reference implementation");

    free(ref);
}
END_TEST

START_TEST(test_recursive_1024) {
    const size_t len = 1024;

    TYPE *nrs = random_array(len, len);
    TYPE *ref = prefix_sums_ref(nrs, len);

    prefix_sums(nrs, len, NULL);

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

    prefix_sums(nrs, len, NULL);

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

int main(int argc __attribute__ ((unused)), char **argv __attribute__ ((unused))) {
    int number_failed;
    Suite *s = recursive_suite();
    SRunner *sr = srunner_create(s);
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
