#include "csv.h"

FILE *csv_open(const char *filename) {
    if (filename == NULL || filename[0] == '\0') {
        return NULL;
    }

    FILE *ret = fopen(filename, "a");
    if (ret == NULL) {
        perror("csv_open");
    }

    return ret;
}

int csv_close(FILE *file) {
    int ret = fclose(file);
    if (ret != 0) {
        perror("csv_close");
    }

    return ret;
}
