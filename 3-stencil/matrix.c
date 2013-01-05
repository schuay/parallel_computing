#include <stdlib.h>
#include <stdio.h>

#include "matrix.h"

#define UPPER_BOUND (500000)

struct __matrix_t {
    int n, m;
    double *elems;
};

matrix_t *matrix_create(int m, int n)
{
    matrix_t *matrix = malloc(sizeof(matrix_t));
    if (matrix == NULL) {
        return NULL;
    }

    matrix->n = n;
    matrix->m = m;

    matrix->elems = malloc((n * m) * sizeof(double));
    if (matrix->elems == NULL) {
        free(matrix);
        return NULL;
    }

    return matrix;
}

matrix_t *matrix_random(int m, int n, int seed)
{
    matrix_t *matrix = matrix_create(m, n);
    if (matrix == NULL) {
        return NULL;
    }

    srand(seed);

    for (int i = 0; i < n * m; i++) {
        matrix->elems[i] = rand() % UPPER_BOUND;
    }

    return matrix;
}

void matrix_free(matrix_t *matrix)
{
    free(matrix->elems);
    free(matrix);
}

static inline int matrix_index(const matrix_t *matrix, int i, int j)
{
    return i * matrix->n + j;
}

double matrix_get(const matrix_t *matrix, int i, int j)
{
    return matrix->elems[matrix_index(matrix, i, j)];
}

void matrix_set(matrix_t *matrix, int i, int j, double elem)
{
    matrix->elems[matrix_index(matrix, i, j)] = elem;
}

int matrix_save(const matrix_t *matrix, FILE *file)
{
    int ret = 0;

    fprintf(file, "%d %d\n", matrix->m, matrix->n);

    for (int i = 0; i < matrix->m; i++) {
        for (int j = 0; j < matrix->n; j++) {
            fprintf(file, "%f ", matrix_get(matrix, i, j));
        }
        fprintf(file, "\n");
    }

    return ret;
}
