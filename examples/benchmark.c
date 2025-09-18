#include "../src/core/floyd_warshall.h"
#include "../src/utils/memory_manager.h"
#include "../src/utils/file_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* Create a random graph for benchmarking */
Graph* create_random_graph(int vertices, double density) {
    Graph *graph = graph_create(vertices);
    if (!graph) {
        return NULL;
    }

    srand((unsigned int)time(NULL));

    for (int i = 0; i < vertices; i++) {
        for (int j = 0; j < vertices; j++) {
            if (i != j && (double)rand() / RAND_MAX < density) {
                double weight = (double)(rand() % 100 + 1);
                graph_add_edge(graph, i, j, weight);
            }
        }
    }

    return graph;
}

/* Benchmark performance for different graph sizes */
void benchmark_performance(void) {
    int sizes[] = {10, 25, 50, 100, 200};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    double density = 0.3; /* 30% edge density */

    printf("Floyd-Warshall Algorithm Performance Benchmark\n");
    printf("==============================================\n\n");
    printf("Graph Density: %.1f%%\n", density * 100);
    printf("%-10s %-15s %-15s %-15s %-15s\n",
           "Vertices", "Std Time(s)", "Opt Time(s)", "Memory(KB)", "Iterations");
    printf("-----------------------------------------------------------------------\n");

    for (int i = 0; i < num_sizes; i++) {
        reset_memory_tracking();

        /* Create random graph */
        Graph *graph1 = create_random_graph(sizes[i], density);
        Graph *graph2 = graph_copy(graph1); /* Copy for optimized version */

        if (!graph1 || !graph2) {
            printf("Failed to create graph with %d vertices\n", sizes[i]);
            continue;
        }

        /* Benchmark standard algorithm */
        FloydWarshallResult std_result = floyd_warshall_execute(graph1);

        /* Benchmark optimized algorithm */
        FloydWarshallResult opt_result = floyd_warshall_execute_optimized(graph2);

        size_t memory_used = get_memory_usage() / 1024; /* Convert to KB */

        printf("%-10d %-15.6f %-15.6f %-15zu %-15d\n",
               sizes[i], std_result.execution_time, opt_result.execution_time,
               memory_used, std_result.iterations);

        graph_destroy(graph1);
        graph_destroy(graph2);
    }

    printf("\nBenchmark completed.\n");
}

/* Test algorithm correctness with known results */
void test_correctness(void) {
    printf("\nAlgorithm Correctness Test\n");
    printf("==========================\n");

    /* Test case 1: Simple 4-vertex graph */
    Graph *graph = graph_create(4);
    graph_add_edge(graph, 0, 1, 3.0);
    graph_add_edge(graph, 0, 3, 7.0);
    graph_add_edge(graph, 1, 0, 8.0);
    graph_add_edge(graph, 1, 2, 2.0);
    graph_add_edge(graph, 2, 0, 5.0);
    graph_add_edge(graph, 2, 3, 1.0);
    graph_add_edge(graph, 3, 0, 2.0);

    FloydWarshallResult result = floyd_warshall_execute(graph);

    if (result.success) {
        printf("Test passed: Standard algorithm\n");

        /* Verify some known shortest distances */
        double dist_0_2 = floyd_warshall_get_distance(graph, 0, 2);
        double dist_1_3 = floyd_warshall_get_distance(graph, 1, 3);

        printf("Distance 0->2: %.2f (expected: 5.00)\n", dist_0_2);
        printf("Distance 1->3: %.2f (expected: 3.00)\n", dist_1_3);

        if (!result.has_negative_cycle) {
            printf("No negative cycle detected (correct)\n");
        }
    } else {
        printf("Test failed: Algorithm execution error\n");
    }

    graph_destroy(graph);

    /* Test case 2: Graph with negative cycle */
    printf("\nTesting negative cycle detection...\n");
    Graph *neg_graph = graph_create(3);
    graph_add_edge(neg_graph, 0, 1, 1.0);
    graph_add_edge(neg_graph, 1, 2, -3.0);
    graph_add_edge(neg_graph, 2, 0, 1.0);

    FloydWarshallResult neg_result = floyd_warshall_execute(neg_graph);

    if (neg_result.success && neg_result.has_negative_cycle) {
        printf("Negative cycle correctly detected\n");
    } else {
        printf("Failed to detect negative cycle\n");
    }

    graph_destroy(neg_graph);
}

/* Compare standard vs optimized algorithm */
void compare_algorithms(void) {
    printf("\nAlgorithm Comparison\n");
    printf("===================\n");

    int test_size = 50;
    double density = 0.4;

    Graph *graph1 = create_random_graph(test_size, density);
    Graph *graph2 = graph_copy(graph1);

    if (!graph1 || !graph2) {
        printf("Failed to create test graphs\n");
        return;
    }

    printf("Graph size: %d vertices, %.1f%% density\n", test_size, density * 100);

    /* Run standard algorithm */
    FloydWarshallResult std_result = floyd_warshall_execute(graph1);

    /* Run optimized algorithm */
    FloydWarshallResult opt_result = floyd_warshall_execute_optimized(graph2);

    printf("\nResults comparison:\n");
    printf("Standard algorithm:\n");
    printf("  Time: %.6f seconds\n", std_result.execution_time);
    printf("  Iterations: %d\n", std_result.iterations);

    printf("Optimized algorithm:\n");
    printf("  Time: %.6f seconds\n", opt_result.execution_time);
    printf("  Iterations: %d\n", opt_result.iterations);

    if (opt_result.execution_time > 0) {
        double speedup = std_result.execution_time / opt_result.execution_time;
        printf("  Speedup: %.2fx\n", speedup);
    }

    /* Verify results are identical */
    bool results_match = true;
    for (int i = 0; i < graph1->vertices && results_match; i++) {
        for (int j = 0; j < graph1->vertices && results_match; j++) {
            if (fabs(graph1->distance[i][j] - graph2->distance[i][j]) > EPSILON) {
                results_match = false;
            }
        }
    }

    printf("Results match: %s\n", results_match ? "Yes" : "No");

    graph_destroy(graph1);
    graph_destroy(graph2);
}

int main(void) {
    printf("Floyd-Warshall Algorithm Benchmark Suite\n");
    printf("========================================\n\n");

    /* Run tests */
    test_correctness();
    benchmark_performance();
    compare_algorithms();

    printf("\nBenchmark suite completed.\n");
    return 0;
}