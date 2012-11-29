#include <omp.h>
#include <stdio.h>

int main(int argc, char **argv) {
    printf("omp_get_max_threads() == %d\n", omp_get_max_threads());
    return 0;
}
