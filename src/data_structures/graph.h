#ifndef GRAPH_H
#define GRAPH_H

#include "../core/constants.h"

/* Graph structure for Floyd-Warshall algorithm */
typedef struct {
    int vertices;           /* Number of vertices in the graph */
    double **distance;      /* Distance matrix */
    int **next;            /* Next vertex matrix for path reconstruction */
    bool is_initialized;    /* Flag to check if graph is properly initialized */
} Graph;

/* Graph creation and destruction functions */
Graph* graph_create(int vertices);
void graph_destroy(Graph *graph);

/* Edge management functions */
ReturnCode graph_add_edge(Graph *graph, int from, int to, double weight);
double graph_get_edge(const Graph *graph, int from, int to);
bool graph_has_edge(const Graph *graph, int from, int to);

/* Graph utility functions */
ReturnCode graph_validate(const Graph *graph);
Graph* graph_copy(const Graph *original);
void graph_print(const Graph *graph);
void graph_print_distances(const Graph *graph);

/* Graph initialization helper */
ReturnCode graph_initialize(Graph *graph);

#endif /* GRAPH_H */