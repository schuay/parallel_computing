#include <check.h>
#include <stdlib.h>

#include "merge.h"

#define LARGE_N (500000)
#define MERGE_TEST(a, b) \
    do { \
        int nproc; \
        size_t n = sizeof(a) / sizeof(a[0]); \
        size_t m = sizeof(b) / sizeof(b[0]); \
 \
        qsort(a, n, sizeof(a[0]), less_than); \
        qsort(b, n, sizeof(a[0]), less_than); \
 \
        TYPE *ref = merge_ref(a, n, b, m); \
        TYPE *tst = merge(a, n, b, m, NULL, "0", &nproc); \
 \
        fail_unless(memcmp(ref, tst, sizeof(TYPE) * (n + m)) == 0, \
                "Result not equal to reference implementation"); \
 \
        free(ref); \
        free(tst); \
    } while (0);

static int less_than(const void *a, const void *b) {
    TYPE *_a = (TYPE *)a;
    TYPE *_b = (TYPE *)b;
    return (int)(*_a - *_b);
}

START_TEST(test_singles)
{
    TYPE a[] = {0};
    TYPE b[] = {1};
    MERGE_TEST(a, b);
}
END_TEST

START_TEST(test_singles_reversed)
{
    TYPE a[] = {1};
    TYPE b[] = {0};
    MERGE_TEST(a, b);
}
END_TEST

START_TEST(test_a_lt_b)
{
    TYPE a[] = {6,1,2,7,3,4,8,5,9};
    TYPE b[] = {16, 11, 12, 17, 13, 14, 18, 16, 19};
    MERGE_TEST(a, b);
}
END_TEST

START_TEST(test_b_lt_a)
{
    TYPE a[] = {16, 11, 12, 17, 13, 14, 18, 16, 19};
    TYPE b[] = {6,1,2,7,3,4,8,5,9};
    MERGE_TEST(a, b);
}
END_TEST

START_TEST(test_interleaved)
{
    TYPE a[] = {0, 2, 4, 8, 10};
    TYPE b[] = {1, 3, 5, 9, 11};
    MERGE_TEST(a, b);
}
END_TEST

START_TEST(test_block_interleaved)
{
    TYPE a[] = {0, 2, 4, 20, 22, 24, 8, 10};
    TYPE b[] = {1, 3, 5, 21, 23, 25, 9, 11};
    MERGE_TEST(a, b);
}
END_TEST

START_TEST(test_interleaved_large)
{
    TYPE *a = calloc(LARGE_N, sizeof(TYPE));
    TYPE *b = calloc(LARGE_N, sizeof(TYPE));
    int i;

    for (i = 0; i < LARGE_N; i++) a[i] = i * 2;
    for (i = 0; i < LARGE_N; i++) b[i] = i * 2 + 1;

    MERGE_TEST(a, b);

    free(a);
    free(b);
}
END_TEST

START_TEST(test_random_large)
{
    TYPE *a = calloc(LARGE_N, sizeof(TYPE));
    TYPE *b = calloc(LARGE_N, sizeof(TYPE));

    /* Get two random sequences of unique numbers by first filling
     * an array with the available number set, then picking numbers
     * from this array in some random order. */

    int k = LARGE_N * 2;
    TYPE *c = calloc(k, sizeof(TYPE));
    int i;

    for (i = 0; i < k; i++) c[i] = i;

    srand(42);

    for (i = 0; i < LARGE_N; i++) {
        int j = rand() % k;
        a[i] = c[j];
        c[j] = c[--k];
    }
    for (i = 0; i < LARGE_N; i++) {
        int j = rand() % k;
        b[i] = c[j];
        c[j] = c[--k];
    }

    free(c);

    MERGE_TEST(a, b);

    free(a);
    free(b);
}
END_TEST

static Suite *create_suite(void)
{
    Suite *s = suite_create("merge");
    TCase *tc_core = tcase_create("core");

    tcase_add_test(tc_core, test_singles);
    tcase_add_test(tc_core, test_singles_reversed);
    tcase_add_test(tc_core, test_a_lt_b);
    tcase_add_test(tc_core, test_b_lt_a);
    tcase_add_test(tc_core, test_interleaved);
    tcase_add_test(tc_core, test_block_interleaved);
    tcase_add_test(tc_core, test_interleaved_large);
    tcase_add_test(tc_core, test_random_large);

    suite_add_tcase(s, tc_core);

    return s;
}

int main(int argc __attribute__ ((unused)), char **argv __attribute__ ((unused)))
{
    int number_failed;
    Suite *s = create_suite();
    SRunner *sr = srunner_create(s);
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
