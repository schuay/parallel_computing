#include <check.h>
#include <mpi.h>
#include <stdio.h>

#include "matrix.h"
#include "stencil.h"

#define DIM_M (1000)
#define DIM_N (1000)
#define SEED (5)

/* BLOCK_C must be chosen such that it:
 * - evenly divides the process count
 * - evenly divides DIM_N
 * - and the (process count / BLOCK_C) evenly divides DIM_M. */
#define BLOCK_C (2) 

static matrix_t *load_matrix(int m, int n, int seed, int iteration)
{
    char *filename;
    int bytes = asprintf(&filename, "../data/matrix_%dx%d_%d_%d",
            m, n, seed, iteration);
    if (bytes == -1) {
        return NULL;
    }

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        return NULL;
    }

    matrix_t *matrix = matrix_load(file);

    fclose(file);

    return matrix;
}

void stencil_test(int iters, int r, int c)
{
    matrix_t *matrix = load_matrix(DIM_M, DIM_N, SEED, 0);
    matrix_t *result = load_matrix(DIM_M, DIM_N, SEED, iters);

    if (matrix == NULL || result == NULL) {
        fprintf(stderr,
                "Matrix could not be loaded from file, run: build/stencil_ref /dev/null %d %d %d %d\n",
                DIM_M, DIM_N, SEED, iters);
        fail("Could not load matrix from file");
        goto out;
    }

    stencil(matrix, iters, r, c, NULL);

    fail_unless(matrix_equals(matrix, result));

out:
    matrix_free(matrix);
    matrix_free(result);
}

START_TEST(test_cols_100)
{
    int p;
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    stencil_test(100, 1, p);
}
END_TEST

START_TEST(test_cols_300)
{
    int p;
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    stencil_test(300, 1, p);
}
END_TEST

START_TEST(test_cols_500)
{
    int p;
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    stencil_test(500, 1, p);
}
END_TEST

START_TEST(test_rows_100)
{
    int p;
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    stencil_test(100, p, 1);
}
END_TEST

START_TEST(test_rows_300)
{
    int p;
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    stencil_test(300, p, 1);
}
END_TEST

START_TEST(test_rows_500)
{
    int p;
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    stencil_test(500, p, 1);
}
END_TEST

START_TEST(test_blck_100)
{
    int p;
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    stencil_test(100, p / BLOCK_C, BLOCK_C);
}
END_TEST

START_TEST(test_blck_300)
{
    int p;
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    stencil_test(300, p / BLOCK_C, BLOCK_C);
}
END_TEST

START_TEST(test_blck_500)
{
    int p;
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    stencil_test(500, p / BLOCK_C, BLOCK_C);
}
END_TEST

static Suite *create_suite(void)
{
    Suite *s = suite_create("merge");
    TCase *tc_core = tcase_create("core");

    tcase_add_test(tc_core, test_cols_100);
    tcase_add_test(tc_core, test_cols_300);
    tcase_add_test(tc_core, test_cols_500);
    tcase_add_test(tc_core, test_rows_100);
    tcase_add_test(tc_core, test_rows_300);
    tcase_add_test(tc_core, test_rows_500);
    tcase_add_test(tc_core, test_blck_100);
    tcase_add_test(tc_core, test_blck_300);
    tcase_add_test(tc_core, test_blck_500);

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

    MPI_Finalize();

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
