#include <check.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#include "arrayscan.h"
#include "common.h"

static int seed = 42;

#define ARRAYSCAN_TEST(nrs) \
    do { \
        size_t len = sizeof(nrs) / sizeof(nrs[0]); \
        \
        TYPE *tst = arrayscan(nrs, len, MPI_COMM_WORLD); \
        \
        int rank; \
        MPI_Comm_rank(MPI_COMM_WORLD, &rank); \
        if (rank == MASTER) { \
            TYPE *ref = arrayscan_seq(nrs, len); \
            fail_unless(memcmp(tst, ref, len) == 0, \
                 "Result not equal to reference implementation"); \
            \
            free(tst); \
            free(ref); \
        } \
    } while(0);

START_TEST(test_odd) {
    TYPE nrs[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };

    ARRAYSCAN_TEST(nrs);
}
END_TEST

START_TEST(test_even) {
    TYPE nrs[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    ARRAYSCAN_TEST(nrs);
}
END_TEST

START_TEST(test_single) {
    TYPE nrs[] = { 0 };

    ARRAYSCAN_TEST(nrs);
}
END_TEST

START_TEST(test_1024) {
    const size_t size = 1024;

    TYPE *nrs = random_array(size, seed);

    ARRAYSCAN_TEST(nrs);
}
END_TEST

START_TEST(test_16384) {
    const size_t size = 16384;

    TYPE *nrs = random_array(size, seed);

    ARRAYSCAN_TEST(nrs);
}
END_TEST

static Suite *create_suite(void) {
    Suite *s = suite_create(getAlgorithmName());

    TCase *tc_core = tcase_create("core");

    tcase_add_test(tc_core, test_odd);
    tcase_add_test(tc_core, test_even);
    tcase_add_test(tc_core, test_single);
    tcase_add_test(tc_core, test_1024);
    tcase_add_test(tc_core, test_16384);

    suite_add_tcase(s, tc_core);

    return s;
}

int main(int argc, char **argv)
{
    int ret = 0;

    MPI_Init(&argc, &argv);

    if (argc != 2) {
        fprintf(stderr, "Usage: test <seed>\n");
        ret = -1;
        goto out;
    }

    seed = safe_strtol(argv[1]);

    int number_failed = 1;
    Suite *s = create_suite();
    SRunner *sr = srunner_create(s);
    srunner_run_all(sr, CK_NORMAL);

    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    ret = (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;

out:
    MPI_Finalize();

    return ret;
}
