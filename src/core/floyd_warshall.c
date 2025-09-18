#include "floyd_warshall.h"
#include "../utils/memory_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

FloydWarshallResult floyd_warshall_execute(Graph *graph) {
    FloydWarshallResult result = {false, 0.0, 0, false, -1};

    if (!graph || !graph->is_initialized) {
        return result;
    }

    if (graph_validate(graph) != SUCCESS) {
        return result;
    }

    clock_t start_time = clock();
    int n = graph->vertices;
    int iterations = 0;

    /* Floyd-Warshall algorithm main loop */
    for (int k = 0; k < n; k++) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                iterations++;

                /* Check for overflow and valid paths */
                if (graph->distance[i][k] < INFINITY_VALUE &&
                    graph->distance[k][j] < INFINITY_VALUE) {

                    double new_distance = graph->distance[i][k] + graph->distance[k][j];

                    if (new_distance < graph->distance[i][j]) {
                        graph->distance[i][j] = new_distance;
                        graph->next[i][j] = graph->next[i][k];
                    }
                }
            }
        }
    }

    /* Check for negative cycles */
    for (int i = 0; i < n; i++) {
        if (graph->distance[i][i] < -EPSILON) {
            result.has_negative_cycle = true;
            result.negative_cycle_vertex = i;
            break;
        }
    }

    clock_t end_time = clock();
    result.execution_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    result.iterations = iterations;
    result.success = true;

    return result;
}

int* floyd_warshall_get_path(const Graph *graph, int start, int end, int *path_length) {
    if (!graph || !graph->is_initialized || !path_length ||
        start < 0 || start >= graph->vertices ||
        end < 0 || end >= graph->vertices) {
        if (path_length) *path_length = 0;
        return NULL;
    }

    /* Check if path exists */
    if (graph->distance[start][end] >= INFINITY_VALUE) {
        *path_length = 0;
        return NULL;
    }

    /* Count path length */
    int count = 1;
    int current = start;
    while (current != end) {
        current = graph->next[current][end];
        if (current == -1) {
            *path_length = 0;
            return NULL;
        }
        count++;
    }

    /* Allocate path array */
    int *path = (int*)malloc(count * sizeof(int));
    if (!path) {
        *path_length = 0;
        return NULL;
    }

    /* Build path */
    path[0] = start;
    current = start;
    for (int i = 1; i < count; i++) {
        current = graph->next[current][end];
        path[i] = current;
    }

    *path_length = count;
    return path;
}

double floyd_warshall_get_distance(const Graph *graph, int start, int end) {
    if (!graph || !graph->is_initialized ||
        start < 0 || start >= graph->vertices ||
        end < 0 || end >= graph->vertices) {
        return INFINITY_VALUE;
    }

    return graph->distance[start][end];
}

bool floyd_warshall_detect_negative_cycle(const Graph *graph) {
    if (!graph || !graph->is_initialized) {
        return false;
    }

    for (int i = 0; i < graph->vertices; i++) {
        if (graph->distance[i][i] < -EPSILON) {
            return true;
        }
    }

    return false;
}

void floyd_warshall_print_result(const FloydWarshallResult *result) {
    if (!result) {
        return;
    }

    printf("=== Floyd-Warshall Algorithm Execution Result ===\n");
    printf("Execution successful: %s\n", result->success ? "Yes" : "No");
    printf("Execution time: %.6f seconds\n", result->execution_time);
    printf("Iterations performed: %d\n", result->iterations);
    printf("Negative cycle detected: %s\n", result->has_negative_cycle ? "Yes" : "No");

    if (result->has_negative_cycle && result->negative_cycle_vertex != -1) {
        printf("Negative cycle location: vertex %d\n", result->negative_cycle_vertex);
    }
    printf("\n");
}

void floyd_warshall_print_path(const Graph *graph, int start, int end) {
    if (!graph || !graph->is_initialized) {
        printf("Invalid graph\n");
        return;
    }

    if (start < 0 || start >= graph->vertices ||
        end < 0 || end >= graph->vertices) {
        printf("Invalid start or end vertex\n");
        return;
    }

    double distance = floyd_warshall_get_distance(graph, start, end);
    printf("Shortest distance from %d to %d: ", start, end);

    if (distance >= INFINITY_VALUE) {
        printf("No path exists\n");
        return;
    }

    printf("%.2f\n", distance);

    int path_length;
    int *path = floyd_warshall_get_path(graph, start, end, &path_length);

    if (path) {
        printf("Path: ");
        for (int i = 0; i < path_length; i++) {
            printf("%d", path[i]);
            if (i < path_length - 1) {
                printf(" -> ");
            }
        }
        printf("\n");
        free(path);
    } else {
        printf("Path reconstruction failed\n");
    }
}

FloydWarshallResult floyd_warshall_execute_optimized(Graph *graph) {
    FloydWarshallResult result = {false, 0.0, 0, false, -1};

    if (!graph || !graph->is_initialized) {
        return result;
    }

    if (graph_validate(graph) != SUCCESS) {
        return result;
    }

    clock_t start_time = clock();
    int n = graph->vertices;
    int iterations = 0;
    bool changed = true;

    /* Optimized version with early termination */
    for (int k = 0; k < n && changed; k++) {
        changed = false;

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                iterations++;

                if (graph->distance[i][k] < INFINITY_VALUE &&
                    graph->distance[k][j] < INFINITY_VALUE) {

                    double new_distance = graph->distance[i][k] + graph->distance[k][j];

                    if (new_distance < graph->distance[i][j] - EPSILON) {
                        graph->distance[i][j] = new_distance;
                        graph->next[i][j] = graph->next[i][k];
                        changed = true;
                    }
                }
            }
        }
    }

    /* Check for negative cycles */
    for (int i = 0; i < n; i++) {
        if (graph->distance[i][i] < -EPSILON) {
            result.has_negative_cycle = true;
            result.negative_cycle_vertex = i;
            break;
        }
    }

    clock_t end_time = clock();
    result.execution_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    result.iterations = iterations;
    result.success = true;

    return result;
}