#include "graph.h"
#include "../utils/memory_manager.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

Graph* graph_create(int vertices) {
    if (vertices <= 0 || vertices > MAX_VERTICES) {
        return NULL;
    }

    Graph *graph = (Graph*)malloc(sizeof(Graph));
    if (!graph) {
        return NULL;
    }

    graph->vertices = vertices;
    graph->is_initialized = false;

    /* Allocate distance matrix */
    graph->distance = allocate_2d_double(vertices, vertices);
    if (!graph->distance) {
        free(graph);
        return NULL;
    }

    /* Allocate next matrix for path reconstruction */
    graph->next = allocate_2d_int(vertices, vertices);
    if (!graph->next) {
        deallocate_2d_double(graph->distance, vertices);
        free(graph);
        return NULL;
    }

    /* Initialize the graph */
    if (graph_initialize(graph) != SUCCESS) {
        graph_destroy(graph);
        return NULL;
    }

    return graph;
}

void graph_destroy(Graph *graph) {
    if (!graph) {
        return;
    }

    if (graph->distance) {
        deallocate_2d_double(graph->distance, graph->vertices);
    }

    if (graph->next) {
        deallocate_2d_int(graph->next, graph->vertices);
    }

    free(graph);
}

ReturnCode graph_initialize(Graph *graph) {
    if (!graph) {
        return ERROR_NULL_POINTER;
    }

    /* Initialize distance matrix */
    for (int i = 0; i < graph->vertices; i++) {
        for (int j = 0; j < graph->vertices; j++) {
            if (i == j) {
                graph->distance[i][j] = 0.0;
            } else {
                graph->distance[i][j] = INFINITY_VALUE;
            }
            graph->next[i][j] = -1;
        }
    }

    graph->is_initialized = true;
    return SUCCESS;
}

ReturnCode graph_add_edge(Graph *graph, int from, int to, double weight) {
    if (!graph || !graph->is_initialized) {
        return ERROR_NULL_POINTER;
    }

    if (from < 0 || from >= graph->vertices ||
        to < 0 || to >= graph->vertices) {
        return ERROR_INVALID_INPUT;
    }

    graph->distance[from][to] = weight;
    graph->next[from][to] = to;

    return SUCCESS;
}

double graph_get_edge(const Graph *graph, int from, int to) {
    if (!graph || !graph->is_initialized ||
        from < 0 || from >= graph->vertices ||
        to < 0 || to >= graph->vertices) {
        return INFINITY_VALUE;
    }

    return graph->distance[from][to];
}

bool graph_has_edge(const Graph *graph, int from, int to) {
    if (!graph || !graph->is_initialized ||
        from < 0 || from >= graph->vertices ||
        to < 0 || to >= graph->vertices) {
        return false;
    }

    return graph->distance[from][to] < INFINITY_VALUE;
}

ReturnCode graph_validate(const Graph *graph) {
    if (!graph) {
        return ERROR_NULL_POINTER;
    }

    if (!graph->is_initialized) {
        return ERROR_INVALID_INPUT;
    }

    if (graph->vertices <= 0 || graph->vertices > MAX_VERTICES) {
        return ERROR_INVALID_INPUT;
    }

    if (!graph->distance || !graph->next) {
        return ERROR_NULL_POINTER;
    }

    /* Check diagonal elements are zero */
    for (int i = 0; i < graph->vertices; i++) {
        if (fabs(graph->distance[i][i]) > EPSILON) {
            return ERROR_INVALID_INPUT;
        }
    }

    return SUCCESS;
}

Graph* graph_copy(const Graph *original) {
    if (!original || !original->is_initialized) {
        return NULL;
    }

    Graph *copy = graph_create(original->vertices);
    if (!copy) {
        return NULL;
    }

    /* Copy distance matrix */
    for (int i = 0; i < original->vertices; i++) {
        for (int j = 0; j < original->vertices; j++) {
            copy->distance[i][j] = original->distance[i][j];
            copy->next[i][j] = original->next[i][j];
        }
    }

    return copy;
}

void graph_print(const Graph *graph) {
    if (!graph || !graph->is_initialized) {
        printf("Invalid or uninitialized graph\n");
        return;
    }

    printf("Graph with %d vertices:\n", graph->vertices);
    printf("Adjacency Matrix (weights):\n");

    /* Print header */
    printf("      ");
    for (int j = 0; j < graph->vertices; j++) {
        printf("%8d", j);
    }
    printf("\n");

    /* Print rows */
    for (int i = 0; i < graph->vertices; i++) {
        printf("%4d: ", i);
        for (int j = 0; j < graph->vertices; j++) {
            if (graph->distance[i][j] >= INFINITY_VALUE) {
                printf("     INF");
            } else {
                printf("%8.2f", graph->distance[i][j]);
            }
        }
        printf("\n");
    }
    printf("\n");
}

void graph_print_distances(const Graph *graph) {
    if (!graph || !graph->is_initialized) {
        printf("Invalid or uninitialized graph\n");
        return;
    }

    printf("=== Shortest Distance Matrix ===\n");
    printf("      ");
    for (int j = 0; j < graph->vertices; j++) {
        printf("%8d", j);
    }
    printf("\n");

    for (int i = 0; i < graph->vertices; i++) {
        printf("%4d: ", i);
        for (int j = 0; j < graph->vertices; j++) {
            if (graph->distance[i][j] >= INFINITY_VALUE) {
                printf("     INF");
            } else {
                printf("%8.2f", graph->distance[i][j]);
            }
        }
        printf("\n");
    }
    printf("\n");
}