#include <check.h>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>

#include "bucket.h"

#define LARGE_N (500000)
#define UPPER_BOUND (400000)

#define BUCKET_TEST(a, n, upper_bound) \
    do { \
        TYPE *tst = bucket_sort(a, n, upper_bound, NULL); \
        qsort(a, n, sizeof(TYPE), less_than); \
 \
        fail_unless(memcmp(a, tst, sizeof(TYPE) * n) == 0, \
                "Result not equal to reference implementation"); \
 \
        free(tst); \
    } while (0);

static int less_than(const void *a, const void *b) {
    TYPE *_a = (TYPE *)a;
    TYPE *_b = (TYPE *)b;
    return (int)(*_a - *_b);
}

START_TEST(test_singles)
{
    TYPE a[] = {0, 1, 2, 4};
    int n = sizeof(a) / sizeof(a[0]);
    BUCKET_TEST(a, n, 4);
}
END_TEST

START_TEST(test_singles_reversed)
{
    TYPE a[] = {3, 2, 1, 0};
    int n = sizeof(a) / sizeof(a[0]);
    BUCKET_TEST(a, n, 4);
}
END_TEST

START_TEST(test_odd)
{
    TYPE a[] = {6,1,2,7,3,4,8,5,9,16,11,12,17,13,14,18,16,19,13};
    int n = sizeof(a) / sizeof(a[0]);
    BUCKET_TEST(a, n, 20);
}
END_TEST

START_TEST(test_a_lt_b)
{
    TYPE a[] = {6,1,2,7,3,4,8,5,9,16,11,12,17,13,14,18,16,19};
    int n = sizeof(a) / sizeof(a[0]);
    BUCKET_TEST(a, n, 20);
}
END_TEST

START_TEST(test_b_lt_a)
{
    TYPE a[] = {16,11,12,17,13,14,18,16,19,6,1,2,7,3,4,8,5,9};
    int n = sizeof(a) / sizeof(a[0]);
    BUCKET_TEST(a, n, 20);
}
END_TEST

START_TEST(test_interleaved)
{
    TYPE a[] = {0, 2, 4, 8, 10, 1, 3, 5, 9, 11};
    int n = sizeof(a) / sizeof(a[0]);
    BUCKET_TEST(a, n, 12);
}
END_TEST

START_TEST(test_block_interleaved)
{
    TYPE a[] = {0, 2, 4, 20, 22, 24, 8, 10, 1, 3, 5, 21, 23, 25, 9, 11};
    int n = sizeof(a) / sizeof(a[0]);
    BUCKET_TEST(a, n, 26);
}
END_TEST

START_TEST(test_random_large)
{
    int seed = time(NULL);
    MPI_Bcast(&seed, 1, MPI_INT, 0, MPI_COMM_WORLD);

    TYPE *a = random_array(LARGE_N, UPPER_BOUND, seed);

    BUCKET_TEST(a, LARGE_N, UPPER_BOUND);

    free(a);
}
END_TEST

static Suite *create_suite(void)
{
    Suite *s = suite_create("merge");
    TCase *tc_core = tcase_create("core");

    tcase_add_test(tc_core, test_singles);
    tcase_add_test(tc_core, test_singles_reversed);
    tcase_add_test(tc_core, test_odd);
    tcase_add_test(tc_core, test_a_lt_b);
    tcase_add_test(tc_core, test_b_lt_a);
    tcase_add_test(tc_core, test_interleaved);
    tcase_add_test(tc_core, test_block_interleaved);
    tcase_add_test(tc_core, test_random_large);

    suite_add_tcase(s, tc_core);

    return s;
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int number_failed;
    Suite *s = create_suite();
    SRunner *sr = srunner_create(s);
    srunner_run_all(sr, CK_NORMAL);

    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    /* TODO: MPI_Finalize() causes a segfault for some reason.. */
    MPI_Finalize();

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
