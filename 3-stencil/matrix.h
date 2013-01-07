#ifndef __MATRIX_H
#define __MATRIX_H

#include <stdio.h>

struct __matrix_t;
typedef struct __matrix_t matrix_t;

/**
 * In addition to storing a matrix, this struct also
 * stores the starting coordinates corresponding to the top
 * left element within the supermatrix.
 */
typedef struct {
    int n, m, i, j;
    double *elems;
} submatrix_t;

matrix_t *matrix_create(int m, int n);
matrix_t *matrix_random(int m, int n, int seed);
void matrix_free(matrix_t *matrix);

void matrix_dims(const matrix_t *matrix, int *m, int *n);
double matrix_get(const matrix_t *matrix, int i, int j);
void matrix_set(matrix_t *matrix, int i, int j, double elem);

int matrix_equals(const matrix_t *a, const matrix_t *b);

int matrix_save(const matrix_t *matrix, FILE *file);
matrix_t *matrix_load(FILE *file);

submatrix_t *matrix_extract(const matrix_t *matrix, int m, int n, int i, int j);
int matrix_cram(matrix_t *matrix, const submatrix_t *submatrix);

int submatrix_index(const submatrix_t *matrix, int i, int j);
void submatrix_free(submatrix_t *submatrix);

#endif /* __MATRIX_H */
