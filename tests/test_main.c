#include "test_framework.h"
#include "../src/core/floyd_warshall.h"
#include "../src/utils/file_io.h"
#include "../src/utils/memory_manager.h"
#include <stdlib.h>

/* Graph tests */
void test_graph_creation(void) {
    printf("\n=== Graph Creation Tests ===\n");

    /* Test normal graph creation */
    Graph *graph = graph_create(5);
    ASSERT(graph != NULL, "Normal graph creation");
    ASSERT_EQUAL(5, graph->vertices, "Vertex count verification");
    ASSERT(graph->is_initialized, "Initialization flag check");

    /* Check diagonal elements are zero */
    for (int i = 0; i < 5; i++) {
        ASSERT_DOUBLE_EQUAL(0.0, graph->distance[i][i], EPSILON, "Diagonal element is zero");
    }

    /* Check non-diagonal elements are infinity */
    ASSERT_DOUBLE_EQUAL(INFINITY_VALUE, graph->distance[0][1], EPSILON, "Non-diagonal element is infinity");

    graph_destroy(graph);

    /* Test invalid inputs */
    ASSERT(graph_create(0) == NULL, "Zero vertices graph creation fails");
    ASSERT(graph_create(-1) == NULL, "Negative vertices graph creation fails");
    ASSERT(graph_create(MAX_VERTICES + 1) == NULL, "Too many vertices graph creation fails");
}

void test_graph_edge_operations(void) {
    printf("\n=== Graph Edge Operations Tests ===\n");

    Graph *graph = graph_create(4);
    ASSERT(graph != NULL, "Test graph creation");

    /* Test edge addition */
    ASSERT_EQUAL(SUCCESS, graph_add_edge(graph, 0, 1, 5.0), "Add edge (0,1)");
    ASSERT_EQUAL(SUCCESS, graph_add_edge(graph, 1, 2, 3.0), "Add edge (1,2)");
    ASSERT_EQUAL(SUCCESS, graph_add_edge(graph, 2, 3, 2.0), "Add edge (2,3)");

    /* Test edge weight retrieval */
    ASSERT_DOUBLE_EQUAL(5.0, graph_get_edge(graph, 0, 1), EPSILON, "Edge (0,1) weight");
    ASSERT_DOUBLE_EQUAL(3.0, graph_get_edge(graph, 1, 2), EPSILON, "Edge (1,2) weight");

    /* Test non-existent edge */
    ASSERT_DOUBLE_EQUAL(INFINITY_VALUE, graph_get_edge(graph, 0, 3), EPSILON, "Non-existent edge");

    /* Test invalid edge addition */
    ASSERT_EQUAL(ERROR_INVALID_INPUT, graph_add_edge(graph, -1, 0, 1.0), "Invalid source vertex");
    ASSERT_EQUAL(ERROR_INVALID_INPUT, graph_add_edge(graph, 0, 5, 1.0), "Invalid destination vertex");

    graph_destroy(graph);
}

void run_graph_tests(void) {
    printf("Starting Graph Tests...\n");
    test_graph_creation();
    test_graph_edge_operations();
}

/* Floyd-Warshall algorithm tests */
void test_simple_graph_algorithm(void) {
    printf("\n=== Simple Graph Algorithm Tests ===\n");

    /* Create a 3x3 graph */
    Graph *graph = graph_create(3);
    graph_add_edge(graph, 0, 1, 4.0);
    graph_add_edge(graph, 0, 2, 2.0);
    graph_add_edge(graph, 1, 2, 1.0);

    FloydWarshallResult result = floyd_warshall_execute(graph);

    ASSERT(result.success, "Algorithm execution success");
    ASSERT(!result.has_negative_cycle, "No negative cycle");

    /* Check shortest distances */
    ASSERT_DOUBLE_EQUAL(0.0, floyd_warshall_get_distance(graph, 0, 0), EPSILON, "Distance (0,0)");
    ASSERT_DOUBLE_EQUAL(3.0, floyd_warshall_get_distance(graph, 0, 1), EPSILON, "Distance (0,1)");
    ASSERT_DOUBLE_EQUAL(2.0, floyd_warshall_get_distance(graph, 0, 2), EPSILON, "Distance (0,2)");

    graph_destroy(graph);
}

void test_negative_cycle_detection(void) {
    printf("\n=== Negative Cycle Detection Tests ===\n");

    /* Create graph with negative cycle */
    Graph *graph = graph_create(3);
    graph_add_edge(graph, 0, 1, 1.0);
    graph_add_edge(graph, 1, 2, -3.0);
    graph_add_edge(graph, 2, 0, 1.0);

    FloydWarshallResult result = floyd_warshall_execute(graph);

    ASSERT(result.success, "Algorithm execution success");
    ASSERT(result.has_negative_cycle, "Negative cycle detection");

    graph_destroy(graph);
}

void test_path_reconstruction(void) {
    printf("\n=== Path Reconstruction Tests ===\n");

    Graph *graph = graph_create(4);
    graph_add_edge(graph, 0, 1, 5.0);
    graph_add_edge(graph, 0, 3, 10.0);
    graph_add_edge(graph, 1, 2, 3.0);
    graph_add_edge(graph, 2, 3, 1.0);

    floyd_warshall_execute(graph);

    int path_length;
    int *path = floyd_warshall_get_path(graph, 0, 3, &path_length);

    ASSERT(path != NULL, "Path reconstruction success");
    ASSERT_EQUAL(4, path_length, "Path length verification");

    if (path) {
        ASSERT_EQUAL(0, path[0], "Path start vertex");
        ASSERT_EQUAL(1, path[1], "Path intermediate vertex 1");
        ASSERT_EQUAL(2, path[2], "Path intermediate vertex 2");
        ASSERT_EQUAL(3, path[3], "Path end vertex");
        free(path);
    }

    graph_destroy(graph);
}

void run_floyd_warshall_tests(void) {
    printf("Starting Floyd-Warshall Algorithm Tests...\n");
    test_simple_graph_algorithm();
    test_negative_cycle_detection();
    test_path_reconstruction();
}

/* Memory management tests */
void test_memory_allocation(void) {
    printf("\n=== Memory Allocation Tests ===\n");

    reset_memory_tracking();

    /* Test 2D double array allocation */
    double **arr_double = allocate_2d_double(10, 10);
    ASSERT(arr_double != NULL, "2D double array allocation");

    /* Test 2D int array allocation */
    int **arr_int = allocate_2d_int(5, 5);
    ASSERT(arr_int != NULL, "2D int array allocation");

    /* Check memory usage tracking */
    size_t memory_used = get_memory_usage();
    ASSERT(memory_used > 0, "Memory usage tracking");

    /* Clean up */
    deallocate_2d_double(arr_double, 10);
    deallocate_2d_int(arr_int, 5);
}

void run_memory_tests(void) {
    printf("Starting Memory Management Tests...\n");
    test_memory_allocation();
}

/* File I/O tests */
void test_file_operations(void) {
    printf("\n=== File I/O Tests ===\n");

    /* Generate a sample graph file */
    const char *test_file = "test_graph.txt";
    ReturnCode result = generate_sample_graph_file(test_file, 4, 0.5);
    ASSERT_EQUAL(SUCCESS, result, "Sample graph file generation");

    /* Test file existence */
    ASSERT(file_exists(test_file), "Generated file exists");

    /* Test file validation */
    ASSERT_EQUAL(SUCCESS, validate_graph_file(test_file), "File validation");

    /* Test graph loading */
    Graph *graph = load_graph_from_file(test_file);
    ASSERT(graph != NULL, "Graph loading from file");

    if (graph) {
        ASSERT_EQUAL(4, graph->vertices, "Loaded graph vertex count");
        graph_destroy(graph);
    }

    /* Clean up test file */
    remove(test_file);
}

void run_file_io_tests(void) {
    printf("Starting File I/O Tests...\n");
    test_file_operations();
}

/* Main test function */
int main(void) {
    printf("Floyd-Warshall Algorithm Test Suite\n");
    printf("===================================\n");

    /* Run all test suites */
    run_graph_tests();
    run_floyd_warshall_tests();
    run_memory_tests();
    run_file_io_tests();

    /* Print test summary */
    test_summary();

    /* Return exit code based on test results */
    return (global_test_result.failed_tests == 0) ? 0 : 1;
}