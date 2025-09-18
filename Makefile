# Makefile for Floyd-Warshall Algorithm
# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -g
INCLUDES = -Isrc
LIBS = -lm

# Directories
SRCDIR = src
OBJDIR = build
TESTDIR = tests
EXAMPLEDIR = examples
DATADIR = data

# Source files
CORE_SOURCES = $(SRCDIR)/core/floyd_warshall.c
DATA_SOURCES = $(SRCDIR)/data_structures/graph.c
UTIL_SOURCES = $(SRCDIR)/utils/memory_manager.c $(SRCDIR)/utils/file_io.c
MAIN_SOURCE = $(SRCDIR)/main.c

# Test sources
TEST_FRAMEWORK_SOURCES = $(TESTDIR)/test_framework.c
TEST_MAIN_SOURCE = $(TESTDIR)/test_main.c

# Example sources
SIMPLE_EXAMPLE_SOURCE = $(EXAMPLEDIR)/simple_example.c
BENCHMARK_SOURCE = $(EXAMPLEDIR)/benchmark.c

# All library sources (excluding main)
LIB_SOURCES = $(CORE_SOURCES) $(DATA_SOURCES) $(UTIL_SOURCES)
LIB_OBJECTS = $(LIB_SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# Main object
MAIN_OBJECT = $(MAIN_SOURCE:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# Test objects
TEST_FRAMEWORK_OBJECT = $(TEST_FRAMEWORK_SOURCES:$(TESTDIR)/%.c=$(OBJDIR)/tests/%.o)
TEST_MAIN_OBJECT = $(TEST_MAIN_SOURCE:$(TESTDIR)/%.c=$(OBJDIR)/tests/%.o)

# Example objects
SIMPLE_EXAMPLE_OBJECT = $(SIMPLE_EXAMPLE_SOURCE:$(EXAMPLEDIR)/%.c=$(OBJDIR)/examples/%.o)
BENCHMARK_OBJECT = $(BENCHMARK_SOURCE:$(EXAMPLEDIR)/%.c=$(OBJDIR)/examples/%.o)

# Executable names
TARGET = floyd_warshall
TEST_TARGET = test_floyd_warshall
SIMPLE_EXAMPLE_TARGET = simple_example
BENCHMARK_TARGET = benchmark

# Default target
all: $(TARGET)

# Main executable
$(TARGET): $(LIB_OBJECTS) $(MAIN_OBJECT) | $(OBJDIR)
	$(CC) $(LIB_OBJECTS) $(MAIN_OBJECT) -o $@ $(LIBS)

# Test executable
test: $(TEST_TARGET)

$(TEST_TARGET): $(LIB_OBJECTS) $(TEST_FRAMEWORK_OBJECT) $(TEST_MAIN_OBJECT) | $(OBJDIR)
	$(CC) $(LIB_OBJECTS) $(TEST_FRAMEWORK_OBJECT) $(TEST_MAIN_OBJECT) -o $@ $(LIBS)

# Example executables
examples: $(SIMPLE_EXAMPLE_TARGET) $(BENCHMARK_TARGET)

$(SIMPLE_EXAMPLE_TARGET): $(LIB_OBJECTS) $(SIMPLE_EXAMPLE_OBJECT) | $(OBJDIR)
	$(CC) $(LIB_OBJECTS) $(SIMPLE_EXAMPLE_OBJECT) -o $@ $(LIBS)

$(BENCHMARK_TARGET): $(LIB_OBJECTS) $(BENCHMARK_OBJECT) | $(OBJDIR)
	$(CC) $(LIB_OBJECTS) $(BENCHMARK_OBJECT) -o $@ $(LIBS)

# Object file compilation rules
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR)/tests/%.o: $(TESTDIR)/%.c | $(OBJDIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR)/examples/%.o: $(EXAMPLEDIR)/%.c | $(OBJDIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Create build directory structure
$(OBJDIR):
	mkdir -p $(OBJDIR)
	mkdir -p $(OBJDIR)/core
	mkdir -p $(OBJDIR)/data_structures
	mkdir -p $(OBJDIR)/utils
	mkdir -p $(OBJDIR)/tests
	mkdir -p $(OBJDIR)/examples

# Build configurations
debug: CFLAGS += -DDEBUG -g3 -O0
debug: $(TARGET)

release: CFLAGS += -DNDEBUG -O3
release: clean $(TARGET)

# Run tests
run-tests: $(TEST_TARGET)
	./$(TEST_TARGET)

# Run simple example
run-example: $(SIMPLE_EXAMPLE_TARGET)
	./$(SIMPLE_EXAMPLE_TARGET)

# Run benchmark
run-benchmark: $(BENCHMARK_TARGET)
	./$(BENCHMARK_TARGET)

# Demo with sample data
demo: $(TARGET)
	./$(TARGET) -v -o results.txt $(DATADIR)/sample_graph.txt
	@echo "Demo completed. Results saved to results.txt"

# Demo with negative cycle detection
demo-negative: $(TARGET)
	./$(TARGET) -v $(DATADIR)/negative_cycle_graph.txt

# Clean build files
clean:
	rm -rf $(OBJDIR)
	rm -f $(TARGET) $(TEST_TARGET) $(SIMPLE_EXAMPLE_TARGET) $(BENCHMARK_TARGET)
	rm -f results.txt test_graph.txt *.o

# Install to system (requires appropriate permissions)
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

# Uninstall from system
uninstall:
	rm -f /usr/local/bin/$(TARGET)

# Check for memory leaks (requires valgrind)
memcheck: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET) $(DATADIR)/sample_graph.txt

# Code style check (requires cppcheck)
stylecheck:
	cppcheck --enable=all --std=c99 $(SRCDIR)/

# Generate documentation (requires doxygen)
docs:
	doxygen Doxyfile

# Package source code
package: clean
	tar -czf floyd-warshall-src.tar.gz --exclude='.git*' --exclude='*.tar.gz' .

# Help target
help:
	@echo "Available targets:"
	@echo "  all           - Build main program (default)"
	@echo "  test          - Build test program"
	@echo "  examples      - Build example programs"
	@echo "  debug         - Build with debug flags"
	@echo "  release       - Build optimized release version"
	@echo "  run-tests     - Build and run tests"
	@echo "  run-example   - Build and run simple example"
	@echo "  run-benchmark - Build and run benchmark"
	@echo "  demo          - Run demo with sample data"
	@echo "  demo-negative - Run demo with negative cycle detection"
	@echo "  clean         - Remove build files"
	@echo "  install       - Install to system"
	@echo "  uninstall     - Remove from system"
	@echo "  memcheck      - Run memory leak check (requires valgrind)"
	@echo "  stylecheck    - Run code style check (requires cppcheck)"
	@echo "  docs          - Generate documentation (requires doxygen)"
	@echo "  package       - Create source code package"
	@echo "  help          - Show this help message"

# Declare phony targets
.PHONY: all test examples debug release run-tests run-example run-benchmark demo demo-negative clean install uninstall memcheck stylecheck docs package help