#ifndef FILE_IO_H
#define FILE_IO_H

#include "../data_structures/graph.h"

/* Graph file I/O functions */
Graph* load_graph_from_file(const char *filename);
ReturnCode save_graph_to_file(const Graph *graph, const char *filename);
ReturnCode save_result_to_file(const Graph *graph, const char *filename);

/* Utility functions for file handling */
bool file_exists(const char *filename);
ReturnCode validate_graph_file(const char *filename);

/* Sample data generation */
ReturnCode generate_sample_graph_file(const char *filename, int vertices, double density);

#endif /* FILE_IO_H */