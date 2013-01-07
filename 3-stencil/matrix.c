#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "matrix.h"

#define MAX_DELTA (0.1f)
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
    if (matrix == NULL) {
        return;
    }

    free(matrix->elems);
    free(matrix);
}

void matrix_dims(const matrix_t *matrix, int *m, int *n)
{
    *m = matrix->m;
    *n = matrix->n;
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

int matrix_equals(const matrix_t *a, const matrix_t *b)
{
    if (a->m != b->m || a->n != b->n) {
        return 0;
    }
    
    for (int i = 0; i < a->m * a->n; i++) {
        double delta = fabs(a->elems[i] - b->elems[i]);
        if (delta > MAX_DELTA) {
            return 0;
        }
    }    

    return 1;
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

matrix_t *matrix_load(FILE *file)
{
    int m, n;

    int read = fscanf(file, "%d %d\n", &m, &n);
    if (read != 2) {
        return NULL;
    }

    matrix_t *out = matrix_create(m, n);

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            float elem;
            read = fscanf(file, "%f ", &elem);

            if (read != 1) {
                free(out);
                return NULL;
            }

            matrix_set(out, i, j, elem);
        }
        fscanf(file, "\n");
    }

    return out;
}

submatrix_t *matrix_extract(const matrix_t *matrix, int m, int n, int i, int j)
{
    if (m + i > matrix->m || n + j > matrix->n) {
        return NULL;
    }

    submatrix_t *sub = malloc(sizeof(submatrix_t));

    sub->elems = malloc(m * n * sizeof(double));
    if (sub->elems == NULL) {
        free(sub);
        return NULL;
    }

    sub->m = m;
    sub->n = n;
    sub->i = i;
    sub->j = j;

    for (int ii = 0; ii < m; ii++) {
        for (int jj = 0; jj < n; jj++) {
            const int to = submatrix_index(sub, ii, jj);
            sub->elems[to] = matrix_get(matrix, i + ii, j + jj);
        }
    }

    return sub;
}

int matrix_cram(matrix_t *matrix, const submatrix_t *submatrix)
{
    if (submatrix->m + submatrix->i > matrix->m ||
            submatrix->n + submatrix->j > matrix->n) {
        return -1;
    }

    for (int ii = 0; ii < submatrix->m; ii++) {
        for (int jj = 0; jj < submatrix->n; jj++) {
            const int from = submatrix_index(submatrix, ii, jj);
            matrix_set(matrix, ii + submatrix->i, jj + submatrix->j,
                    submatrix->elems[from]);
        }
    }

    return 0;
}

int submatrix_index(const submatrix_t *matrix, int i, int j)
{
    return i * matrix->n + j;
}

void submatrix_free(submatrix_t *submatrix)
{
    if (submatrix == NULL) {
        return;
    }

    free(submatrix->elems);
    free(submatrix);
}
