#ifndef __CSV_H
#define __CSV_H

#include <stdio.h>

FILE *csv_open(char *filename);
int csv_close(FILE *file);

#endif /* __CSV_H */
