#ifndef FLOYD_WARSHALL_H
#define FLOYD_WARSHALL_H

#include "../data_structures/graph.h"
#include <time.h>

/* Result structure for Floyd-Warshall algorithm execution */
typedef struct {
    bool success;              /* Flag indicating successful execution */
    double execution_time;     /* Execution time in seconds */
    int iterations;           /* Number of iterations performed */
    bool has_negative_cycle;  /* Flag indicating presence of negative cycle */
    int negative_cycle_vertex; /* Vertex where negative cycle is detected (-1 if none) */
} FloydWarshallResult;

/* Core algorithm functions */
FloydWarshallResult floyd_warshall_execute(Graph *graph);

/* Path related functions */
int* floyd_warshall_get_path(const Graph *graph, int start, int end, int *path_length);
double floyd_warshall_get_distance(const Graph *graph, int start, int end);

/* Utility functions */
bool floyd_warshall_detect_negative_cycle(const Graph *graph);
void floyd_warshall_print_result(const FloydWarshallResult *result);
void floyd_warshall_print_path(const Graph *graph, int start, int end);

/* Advanced algorithm variants */
FloydWarshallResult floyd_warshall_execute_optimized(Graph *graph);

#endif /* FLOYD_WARSHALL_H */