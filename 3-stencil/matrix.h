#ifndef __MATRIX_H
#define __MATRIX_H

#include <stdio.h>

struct __matrix_t;
typedef struct __matrix_t matrix_t;

matrix_t *matrix_create(int m, int n);
matrix_t *matrix_random(int m, int n, int seed);
void matrix_free(matrix_t *matrix);

void matrix_dims(const matrix_t *matrix, int *m, int *n);
double matrix_get(const matrix_t *matrix, int i, int j);
void matrix_set(matrix_t *matrix, int i, int j, double elem);

int matrix_equals(const matrix_t *a, const matrix_t *b);

int matrix_save(const matrix_t *matrix, FILE *file);
matrix_t *matrix_load(FILE *file);

#endif /* __MATRIX_H */
