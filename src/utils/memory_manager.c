#include "memory_manager.h"
#include <stdlib.h>
#include <stdio.h>

/* Global variables for memory tracking */
static size_t total_allocated = 0;
static size_t current_allocated = 0;
static int allocation_count = 0;

double** allocate_2d_double(int rows, int cols) {
    if (rows <= 0 || cols <= 0) {
        return NULL;
    }

    double **array = (double**)malloc(rows * sizeof(double*));
    if (!array) {
        return NULL;
    }

    for (int i = 0; i < rows; i++) {
        array[i] = (double*)malloc(cols * sizeof(double));
        if (!array[i]) {
            /* Free previously allocated memory on failure */
            for (int j = 0; j < i; j++) {
                free(array[j]);
            }
            free(array);
            return NULL;
        }
    }

    /* Update memory tracking */
    size_t allocated_size = rows * sizeof(double*) + rows * cols * sizeof(double);
    total_allocated += allocated_size;
    current_allocated += allocated_size;
    allocation_count++;

    return array;
}

int** allocate_2d_int(int rows, int cols) {
    if (rows <= 0 || cols <= 0) {
        return NULL;
    }

    int **array = (int**)malloc(rows * sizeof(int*));
    if (!array) {
        return NULL;
    }

    for (int i = 0; i < rows; i++) {
        array[i] = (int*)malloc(cols * sizeof(int));
        if (!array[i]) {
            /* Free previously allocated memory on failure */
            for (int j = 0; j < i; j++) {
                free(array[j]);
            }
            free(array);
            return NULL;
        }
    }

    /* Update memory tracking */
    size_t allocated_size = rows * sizeof(int*) + rows * cols * sizeof(int);
    total_allocated += allocated_size;
    current_allocated += allocated_size;
    allocation_count++;

    return array;
}

void deallocate_2d_double(double **array, int rows) {
    if (!array) {
        return;
    }

    for (int i = 0; i < rows; i++) {
        if (array[i]) {
            free(array[i]);
        }
    }
    free(array);

    /* Update memory tracking */
    size_t deallocated_size = rows * sizeof(double*) + rows * rows * sizeof(double);
    if (current_allocated >= deallocated_size) {
        current_allocated -= deallocated_size;
    }
}

void deallocate_2d_int(int **array, int rows) {
    if (!array) {
        return;
    }

    for (int i = 0; i < rows; i++) {
        if (array[i]) {
            free(array[i]);
        }
    }
    free(array);

    /* Update memory tracking */
    size_t deallocated_size = rows * sizeof(int*) + rows * rows * sizeof(int);
    if (current_allocated >= deallocated_size) {
        current_allocated -= deallocated_size;
    }
}

size_t get_memory_usage(void) {
    return current_allocated;
}

void reset_memory_tracking(void) {
    total_allocated = 0;
    current_allocated = 0;
    allocation_count = 0;
}

void print_memory_stats(void) {
    printf("=== Memory Usage Statistics ===\n");
    printf("Total allocated: %zu bytes\n", total_allocated);
    printf("Currently allocated: %zu bytes\n", current_allocated);
    printf("Allocation count: %d\n", allocation_count);
    printf("\n");
}

void* safe_malloc(size_t size) {
    void *ptr = malloc(size);
    if (!ptr && size > 0) {
        fprintf(stderr, "Memory allocation failed for %zu bytes\n", size);
        exit(ERROR_MEMORY_ALLOCATION);
    }
    return ptr;
}

void* safe_calloc(size_t count, size_t size) {
    void *ptr = calloc(count, size);
    if (!ptr && count > 0 && size > 0) {
        fprintf(stderr, "Memory allocation failed for %zu elements of %zu bytes\n", count, size);
        exit(ERROR_MEMORY_ALLOCATION);
    }
    return ptr;
}