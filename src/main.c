#include "core/floyd_warshall.h"
#include "utils/file_io.h"
#include "utils/memory_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_usage(const char *program_name) {
    printf("Usage: %s [options] <graph_file>\n", program_name);
    printf("Options:\n");
    printf("  -h, --help        Show this help message\n");
    printf("  -v, --verbose     Enable verbose output\n");
    printf("  -o <file>         Save results to file\n");
    printf("  -p <start> <end>  Show specific path\n");
    printf("  -s                Use optimized algorithm\n");
    printf("  -m                Show memory statistics\n");
    printf("\nGraph file format:\n");
    printf("  Line 1: number_of_vertices\n");
    printf("  Line 2: number_of_edges\n");
    printf("  Following lines: from_vertex to_vertex weight\n");
    printf("\nExample:\n");
    printf("  %s -v -o results.txt graph.txt\n", program_name);
    printf("  %s -p 0 3 graph.txt\n", program_name);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    /* Command line argument variables */
    bool verbose = false;
    bool show_memory = false;
    bool use_optimized = false;
    char *input_file = NULL;
    char *output_file = NULL;
    int path_start = -1, path_end = -1;

    /* Parse command line arguments */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            verbose = true;
        } else if (strcmp(argv[i], "-m") == 0) {
            show_memory = true;
        } else if (strcmp(argv[i], "-s") == 0) {
            use_optimized = true;
        } else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            output_file = argv[++i];
        } else if (strcmp(argv[i], "-p") == 0 && i + 2 < argc) {
            path_start = atoi(argv[++i]);
            path_end = atoi(argv[++i]);
        } else if (input_file == NULL) {
            input_file = argv[i];
        } else {
            printf("Error: Unknown argument '%s'\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        }
    }

    if (!input_file) {
        printf("Error: Please specify an input file.\n");
        print_usage(argv[0]);
        return 1;
    }

    /* Initialize memory tracking */
    if (show_memory) {
        reset_memory_tracking();
    }

    /* Validate input file */
    if (validate_graph_file(input_file) != SUCCESS) {
        printf("Error: Invalid graph file format: %s\n", input_file);
        return 1;
    }

    /* Load graph from file */
    if (verbose) {
        printf("Loading graph from file: %s\n", input_file);
    }

    Graph *graph = load_graph_from_file(input_file);
    if (!graph) {
        printf("Error: Failed to load graph from file: %s\n", input_file);
        return 1;
    }

    if (verbose) {
        printf("Graph loaded successfully: %d vertices\n", graph->vertices);
        printf("Initial graph:\n");
        graph_print(graph);
    }

    /* Validate path arguments */
    if (path_start >= 0 || path_end >= 0) {
        if (path_start < 0 || path_start >= graph->vertices ||
            path_end < 0 || path_end >= graph->vertices) {
            printf("Error: Invalid path vertices. Valid range: 0-%d\n", graph->vertices - 1);
            graph_destroy(graph);
            return 1;
        }
    }

    /* Execute Floyd-Warshall algorithm */
    if (verbose) {
        printf("Executing %s Floyd-Warshall algorithm...\n",
               use_optimized ? "optimized" : "standard");
    }

    FloydWarshallResult result;
    if (use_optimized) {
        result = floyd_warshall_execute_optimized(graph);
    } else {
        result = floyd_warshall_execute(graph);
    }

    /* Check execution result */
    if (!result.success) {
        printf("Error: Algorithm execution failed\n");
        graph_destroy(graph);
        return 1;
    }

    /* Print results */
    if (verbose || (!output_file && path_start < 0)) {
        floyd_warshall_print_result(&result);
    }

    if (result.has_negative_cycle) {
        printf("Warning: Negative cycle detected in the graph.\n");
        if (result.negative_cycle_vertex >= 0) {
            printf("Negative cycle involves vertex %d\n", result.negative_cycle_vertex);
        }
    } else {
        /* Show distance matrix if verbose or no specific output requested */
        if (verbose && path_start < 0) {
            graph_print_distances(graph);
        }

        /* Show specific path if requested */
        if (path_start >= 0 && path_end >= 0) {
            floyd_warshall_print_path(graph, path_start, path_end);
        }

        /* Save results to file if requested */
        if (output_file) {
            if (save_result_to_file(graph, output_file) == SUCCESS) {
                printf("Results saved to: %s\n", output_file);
            } else {
                printf("Error: Failed to save results to: %s\n", output_file);
            }
        }
    }

    /* Show memory statistics if requested */
    if (show_memory) {
        print_memory_stats();
    }

    /* Clean up */
    graph_destroy(graph);

    if (verbose) {
        printf("Program completed successfully.\n");
    }

    return 0;
}