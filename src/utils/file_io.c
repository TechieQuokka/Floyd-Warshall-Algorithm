#include "file_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

Graph* load_graph_from_file(const char *filename) {
    if (!filename) {
        return NULL;
    }

    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file %s\n", filename);
        return NULL;
    }

    int vertices;
    if (fscanf(file, "%d", &vertices) != 1 || vertices <= 0) {
        fprintf(stderr, "Error: Invalid number of vertices in file %s\n", filename);
        fclose(file);
        return NULL;
    }

    Graph *graph = graph_create(vertices);
    if (!graph) {
        fprintf(stderr, "Error: Failed to create graph\n");
        fclose(file);
        return NULL;
    }

    int edges;
    if (fscanf(file, "%d", &edges) != 1 || edges < 0) {
        fprintf(stderr, "Error: Invalid number of edges in file %s\n", filename);
        graph_destroy(graph);
        fclose(file);
        return NULL;
    }

    /* Read edges */
    for (int i = 0; i < edges; i++) {
        int from, to;
        double weight;

        if (fscanf(file, "%d %d %lf", &from, &to, &weight) != 3) {
            fprintf(stderr, "Error: Invalid edge format in file %s at line %d\n", filename, i + 3);
            graph_destroy(graph);
            fclose(file);
            return NULL;
        }

        if (graph_add_edge(graph, from, to, weight) != SUCCESS) {
            fprintf(stderr, "Error: Failed to add edge (%d, %d) with weight %.2f\n", from, to, weight);
            graph_destroy(graph);
            fclose(file);
            return NULL;
        }
    }

    fclose(file);
    return graph;
}

ReturnCode save_graph_to_file(const Graph *graph, const char *filename) {
    if (!graph || !filename) {
        return ERROR_NULL_POINTER;
    }

    if (graph_validate(graph) != SUCCESS) {
        return ERROR_INVALID_INPUT;
    }

    FILE *file = fopen(filename, "w");
    if (!file) {
        return ERROR_FILE_IO;
    }

    /* Write number of vertices */
    fprintf(file, "%d\n", graph->vertices);

    /* Count edges */
    int edge_count = 0;
    for (int i = 0; i < graph->vertices; i++) {
        for (int j = 0; j < graph->vertices; j++) {
            if (i != j && graph->distance[i][j] < INFINITY_VALUE) {
                edge_count++;
            }
        }
    }

    /* Write number of edges */
    fprintf(file, "%d\n", edge_count);

    /* Write edges */
    for (int i = 0; i < graph->vertices; i++) {
        for (int j = 0; j < graph->vertices; j++) {
            if (i != j && graph->distance[i][j] < INFINITY_VALUE) {
                fprintf(file, "%d %d %.6f\n", i, j, graph->distance[i][j]);
            }
        }
    }

    fclose(file);
    return SUCCESS;
}

ReturnCode save_result_to_file(const Graph *graph, const char *filename) {
    if (!graph || !filename) {
        return ERROR_NULL_POINTER;
    }

    if (graph_validate(graph) != SUCCESS) {
        return ERROR_INVALID_INPUT;
    }

    FILE *file = fopen(filename, "w");
    if (!file) {
        return ERROR_FILE_IO;
    }

    fprintf(file, "Floyd-Warshall Algorithm Results\n");
    fprintf(file, "================================\n\n");

    fprintf(file, "Graph with %d vertices\n\n", graph->vertices);

    fprintf(file, "Shortest Distance Matrix:\n");
    fprintf(file, "      ");
    for (int j = 0; j < graph->vertices; j++) {
        fprintf(file, "%8d", j);
    }
    fprintf(file, "\n");

    for (int i = 0; i < graph->vertices; i++) {
        fprintf(file, "%4d: ", i);
        for (int j = 0; j < graph->vertices; j++) {
            if (graph->distance[i][j] >= INFINITY_VALUE) {
                fprintf(file, "     INF");
            } else {
                fprintf(file, "%8.2f", graph->distance[i][j]);
            }
        }
        fprintf(file, "\n");
    }

    fprintf(file, "\nPath Information:\n");
    for (int i = 0; i < graph->vertices; i++) {
        for (int j = 0; j < graph->vertices; j++) {
            if (i != j && graph->distance[i][j] < INFINITY_VALUE) {
                fprintf(file, "Path from %d to %d (distance: %.2f): ", i, j, graph->distance[i][j]);

                int path_length;
                int *path = floyd_warshall_get_path(graph, i, j, &path_length);
                if (path) {
                    for (int k = 0; k < path_length; k++) {
                        fprintf(file, "%d", path[k]);
                        if (k < path_length - 1) {
                            fprintf(file, " -> ");
                        }
                    }
                    free(path);
                } else {
                    fprintf(file, "Path reconstruction failed");
                }
                fprintf(file, "\n");
            }
        }
    }

    fclose(file);
    return SUCCESS;
}

bool file_exists(const char *filename) {
    if (!filename) {
        return false;
    }

    FILE *file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return true;
    }
    return false;
}

ReturnCode validate_graph_file(const char *filename) {
    if (!filename) {
        return ERROR_NULL_POINTER;
    }

    if (!file_exists(filename)) {
        return ERROR_FILE_IO;
    }

    FILE *file = fopen(filename, "r");
    if (!file) {
        return ERROR_FILE_IO;
    }

    int vertices, edges;
    if (fscanf(file, "%d %d", &vertices, &edges) != 2) {
        fclose(file);
        return ERROR_INVALID_INPUT;
    }

    if (vertices <= 0 || vertices > MAX_VERTICES || edges < 0) {
        fclose(file);
        return ERROR_INVALID_INPUT;
    }

    /* Validate each edge */
    for (int i = 0; i < edges; i++) {
        int from, to;
        double weight;

        if (fscanf(file, "%d %d %lf", &from, &to, &weight) != 3) {
            fclose(file);
            return ERROR_INVALID_INPUT;
        }

        if (from < 0 || from >= vertices || to < 0 || to >= vertices) {
            fclose(file);
            return ERROR_INVALID_INPUT;
        }
    }

    fclose(file);
    return SUCCESS;
}

ReturnCode generate_sample_graph_file(const char *filename, int vertices, double density) {
    if (!filename || vertices <= 0 || vertices > MAX_VERTICES ||
        density < 0.0 || density > 1.0) {
        return ERROR_INVALID_INPUT;
    }

    FILE *file = fopen(filename, "w");
    if (!file) {
        return ERROR_FILE_IO;
    }

    srand((unsigned int)time(NULL));

    /* Write number of vertices */
    fprintf(file, "%d\n", vertices);

    /* Generate edges based on density */
    int max_edges = vertices * (vertices - 1);
    int target_edges = (int)(max_edges * density);
    int actual_edges = 0;

    /* Temporary storage for edges */
    typedef struct {
        int from, to;
        double weight;
    } Edge;

    Edge *edges = (Edge*)malloc(target_edges * sizeof(Edge));
    if (!edges) {
        fclose(file);
        return ERROR_MEMORY_ALLOCATION;
    }

    /* Generate random edges */
    while (actual_edges < target_edges) {
        int from = rand() % vertices;
        int to = rand() % vertices;

        if (from != to) {
            /* Check if edge already exists */
            bool exists = false;
            for (int i = 0; i < actual_edges; i++) {
                if (edges[i].from == from && edges[i].to == to) {
                    exists = true;
                    break;
                }
            }

            if (!exists) {
                edges[actual_edges].from = from;
                edges[actual_edges].to = to;
                edges[actual_edges].weight = (double)(rand() % 100 + 1);
                actual_edges++;
            }
        }
    }

    /* Write number of edges */
    fprintf(file, "%d\n", actual_edges);

    /* Write edges */
    for (int i = 0; i < actual_edges; i++) {
        fprintf(file, "%d %d %.2f\n", edges[i].from, edges[i].to, edges[i].weight);
    }

    free(edges);
    fclose(file);
    return SUCCESS;
}