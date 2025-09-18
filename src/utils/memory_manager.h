#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stddef.h>
#include "../core/constants.h"

/* 2D array allocation and deallocation functions */
double** allocate_2d_double(int rows, int cols);
int** allocate_2d_int(int rows, int cols);
void deallocate_2d_double(double **array, int rows);
void deallocate_2d_int(int **array, int rows);

/* Memory usage tracking functions */
size_t get_memory_usage(void);
void reset_memory_tracking(void);
void print_memory_stats(void);

/* Safe memory allocation with error checking */
void* safe_malloc(size_t size);
void* safe_calloc(size_t count, size_t size);

#endif /* MEMORY_MANAGER_H */