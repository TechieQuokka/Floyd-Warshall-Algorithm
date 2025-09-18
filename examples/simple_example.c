#include "../src/core/floyd_warshall.h"
#include <stdio.h>

int main(void) {
    printf("Floyd-Warshall Algorithm Simple Example\n");
    printf("=======================================\n\n");

    /* Create a graph with 4 vertices */
    Graph *graph = graph_create(4);
    if (!graph) {
        printf("Error: Failed to create graph\n");
        return 1;
    }

    /* Add edges to the graph */
    printf("Adding edges to the graph...\n");
    graph_add_edge(graph, 0, 1, 5.0);
    graph_add_edge(graph, 0, 3, 10.0);
    graph_add_edge(graph, 1, 2, 3.0);
    graph_add_edge(graph, 2, 3, 1.0);
    graph_add_edge(graph, 3, 1, 2.0);

    /* Print initial graph */
    printf("\nInitial graph:\n");
    graph_print(graph);

    /* Execute Floyd-Warshall algorithm */
    printf("Executing Floyd-Warshall algorithm...\n");
    FloydWarshallResult result = floyd_warshall_execute(graph);

    /* Check execution result */
    if (result.success) {
        printf("Algorithm executed successfully!\n\n");

        /* Print execution statistics */
        floyd_warshall_print_result(&result);

        /* Print shortest distance matrix */
        graph_print_distances(graph);

        /* Show some specific paths */
        printf("Sample shortest paths:\n");
        for (int i = 0; i < graph->vertices; i++) {
            for (int j = 0; j < graph->vertices; j++) {
                if (i != j) {
                    floyd_warshall_print_path(graph, i, j);
                }
            }
        }

        if (result.has_negative_cycle) {
            printf("\nWarning: Negative cycle detected!\n");
        }
    } else {
        printf("Error: Algorithm execution failed\n");
        graph_destroy(graph);
        return 1;
    }

    /* Clean up */
    graph_destroy(graph);
    printf("\nExample completed successfully.\n");
    return 0;
}