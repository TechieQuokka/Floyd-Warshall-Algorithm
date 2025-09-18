#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <limits.h>
#include <float.h>
#include <stdbool.h>

/* Maximum number of vertices in a graph */
#define MAX_VERTICES 1000

/* Infinity representation for unreachable vertices */
#define INFINITY_VALUE 1e9

/* Epsilon for floating point comparisons */
#define EPSILON 1e-9

/* Maximum filename length */
#define MAX_FILENAME_LENGTH 256

/* Maximum line length for file reading */
#define MAX_LINE_LENGTH 1024

/* Return codes for functions */
typedef enum {
    SUCCESS = 0,
    ERROR_MEMORY_ALLOCATION = -1,
    ERROR_INVALID_INPUT = -2,
    ERROR_FILE_IO = -3,
    ERROR_NEGATIVE_CYCLE = -4,
    ERROR_NULL_POINTER = -5
} ReturnCode;

/* Boolean type for older C standards */
#ifndef __cplusplus
#ifndef bool
typedef int bool;
#define true 1
#define false 0
#endif
#endif

#endif /* CONSTANTS_H */