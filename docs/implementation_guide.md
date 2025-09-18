# Floyd-Warshall 알고리즘 구현 가이드

## 목차
1. [프로젝트 구조](#1-프로젝트-구조)
2. [핵심 모듈 구현](#2-핵심-모듈-구현)
3. [빌드 시스템](#3-빌드-시스템)
4. [테스트 전략](#4-테스트-전략)
5. [성능 최적화](#5-성능-최적화)
6. [사용법 가이드](#6-사용법-가이드)

## 1. 프로젝트 구조

### 1.1 디렉토리 구조
```
Floyd-Warshall-Algorithm/
├── src/                      # 소스 코드
│   ├── core/                 # 핵심 알고리즘
│   │   ├── floyd_warshall.h
│   │   ├── floyd_warshall.c
│   │   └── constants.h
│   ├── data_structures/      # 자료구조
│   │   ├── graph.h
│   │   ├── graph.c
│   │   └── matrix.h
│   ├── utils/               # 유틸리티
│   │   ├── memory_manager.h
│   │   ├── memory_manager.c
│   │   ├── file_io.h
│   │   ├── file_io.c
│   │   └── logger.h
│   └── main.c               # 메인 프로그램
├── include/                 # 헤더 파일 (external)
├── tests/                   # 테스트 코드
├── examples/                # 예제 코드
├── data/                    # 테스트 데이터
├── docs/                    # 문서
├── build/                   # 빌드 출력
└── Makefile                 # 빌드 설정
```

### 1.2 헤더 파일 구조
```c
// src/core/constants.h
#ifndef CONSTANTS_H
#define CONSTANTS_H

#define MAX_VERTICES 1000
#define INFINITY 1e9
#define EPSILON 1e-9
#define MAX_FILENAME_LENGTH 256
#define MAX_LINE_LENGTH 1024

// 반환 코드
typedef enum {
    SUCCESS = 0,
    ERROR_MEMORY_ALLOCATION = -1,
    ERROR_INVALID_INPUT = -2,
    ERROR_FILE_IO = -3,
    ERROR_NEGATIVE_CYCLE = -4
} ReturnCode;

#endif
```

## 2. 핵심 모듈 구현

### 2.1 그래프 자료구조 (graph.h/graph.c)

```c
// src/data_structures/graph.h
#ifndef GRAPH_H
#define GRAPH_H

#include <stdbool.h>
#include "../core/constants.h"

typedef struct {
    int vertices;           // 정점의 수
    double **distance;      // 거리 행렬
    int **next;            // 경로 복원용 행렬
    bool is_initialized;    // 초기화 여부
} Graph;

// 그래프 생성 및 삭제
Graph* graph_create(int vertices);
void graph_destroy(Graph *graph);

// 간선 관리
ReturnCode graph_add_edge(Graph *graph, int from, int to, double weight);
double graph_get_edge(const Graph *graph, int from, int to);
bool graph_has_edge(const Graph *graph, int from, int to);

// 그래프 유틸리티
ReturnCode graph_validate(const Graph *graph);
Graph* graph_copy(const Graph *original);
void graph_print(const Graph *graph);

#endif
```

```c
// src/data_structures/graph.c
#include "graph.h"
#include "../utils/memory_manager.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

Graph* graph_create(int vertices) {
    if (vertices <= 0 || vertices > MAX_VERTICES) {
        return NULL;
    }

    Graph *graph = malloc(sizeof(Graph));
    if (!graph) {
        return NULL;
    }

    graph->vertices = vertices;
    graph->is_initialized = false;

    // 거리 행렬 할당
    graph->distance = allocate_2d_double(vertices, vertices);
    if (!graph->distance) {
        free(graph);
        return NULL;
    }

    // 경로 복원 행렬 할당
    graph->next = allocate_2d_int(vertices, vertices);
    if (!graph->next) {
        deallocate_2d_double(graph->distance, vertices);
        free(graph);
        return NULL;
    }

    // 초기화
    for (int i = 0; i < vertices; i++) {
        for (int j = 0; j < vertices; j++) {
            graph->distance[i][j] = (i == j) ? 0.0 : INFINITY;
            graph->next[i][j] = -1;
        }
    }

    graph->is_initialized = true;
    return graph;
}

void graph_destroy(Graph *graph) {
    if (!graph) return;

    if (graph->distance) {
        deallocate_2d_double(graph->distance, graph->vertices);
    }

    if (graph->next) {
        deallocate_2d_int(graph->next, graph->vertices);
    }

    free(graph);
}

ReturnCode graph_add_edge(Graph *graph, int from, int to, double weight) {
    if (!graph || !graph->is_initialized) {
        return ERROR_INVALID_INPUT;
    }

    if (from < 0 || from >= graph->vertices ||
        to < 0 || to >= graph->vertices) {
        return ERROR_INVALID_INPUT;
    }

    graph->distance[from][to] = weight;
    graph->next[from][to] = to;

    return SUCCESS;
}
```

### 2.2 Floyd-Warshall 핵심 알고리즘 (floyd_warshall.h/floyd_warshall.c)

```c
// src/core/floyd_warshall.h
#ifndef FLOYD_WARSHALL_H
#define FLOYD_WARSHALL_H

#include "../data_structures/graph.h"
#include <time.h>

typedef struct {
    bool success;              // 실행 성공 여부
    double execution_time;     // 실행 시간 (초)
    int iterations;           // 수행된 반복 횟수
    bool has_negative_cycle;  // 음의 사이클 존재 여부
    int negative_cycle_vertex; // 음의 사이클이 있는 정점 (-1이면 없음)
} FloydWarshallResult;

// 핵심 알고리즘 함수
FloydWarshallResult floyd_warshall_execute(Graph *graph);

// 경로 관련 함수
int* floyd_warshall_get_path(const Graph *graph, int start, int end, int *path_length);
double floyd_warshall_get_distance(const Graph *graph, int start, int end);

// 유틸리티 함수
bool floyd_warshall_detect_negative_cycle(const Graph *graph);
void floyd_warshall_print_result(const FloydWarshallResult *result);
void floyd_warshall_print_distances(const Graph *graph);

#endif
```

```c
// src/core/floyd_warshall.c
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

    clock_t start_time = clock();
    int n = graph->vertices;
    int iterations = 0;

    // Floyd-Warshall 알고리즘 실행
    for (int k = 0; k < n; k++) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                iterations++;

                // 오버플로우 방지
                if (graph->distance[i][k] < INFINITY &&
                    graph->distance[k][j] < INFINITY) {

                    double new_distance = graph->distance[i][k] + graph->distance[k][j];

                    if (new_distance < graph->distance[i][j]) {
                        graph->distance[i][j] = new_distance;
                        graph->next[i][j] = graph->next[i][k];
                    }
                }
            }
        }
    }

    // 음의 사이클 검출
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
    if (!graph || !graph->is_initialized ||
        start < 0 || start >= graph->vertices ||
        end < 0 || end >= graph->vertices || !path_length) {
        *path_length = 0;
        return NULL;
    }

    // 경로가 존재하지 않는 경우
    if (graph->distance[start][end] >= INFINITY) {
        *path_length = 0;
        return NULL;
    }

    // 경로 길이 계산
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

    // 경로 배열 할당
    int *path = malloc(count * sizeof(int));
    if (!path) {
        *path_length = 0;
        return NULL;
    }

    // 경로 구성
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
        return INFINITY;
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
    if (!result) return;

    printf("=== Floyd-Warshall 알고리즘 실행 결과 ===\n");
    printf("실행 성공: %s\n", result->success ? "예" : "아니오");
    printf("실행 시간: %.6f초\n", result->execution_time);
    printf("반복 횟수: %d\n", result->iterations);
    printf("음의 사이클: %s\n", result->has_negative_cycle ? "존재" : "없음");

    if (result->has_negative_cycle && result->negative_cycle_vertex != -1) {
        printf("음의 사이클 위치: 정점 %d\n", result->negative_cycle_vertex);
    }
    printf("\n");
}

void floyd_warshall_print_distances(const Graph *graph) {
    if (!graph || !graph->is_initialized) {
        printf("유효하지 않은 그래프입니다.\n");
        return;
    }

    printf("=== 최단 거리 행렬 ===\n");
    printf("      ");
    for (int j = 0; j < graph->vertices; j++) {
        printf("%8d", j);
    }
    printf("\n");

    for (int i = 0; i < graph->vertices; i++) {
        printf("%4d: ", i);
        for (int j = 0; j < graph->vertices; j++) {
            if (graph->distance[i][j] >= INFINITY) {
                printf("     INF");
            } else {
                printf("%8.2f", graph->distance[i][j]);
            }
        }
        printf("\n");
    }
    printf("\n");
}
```

### 2.3 메모리 관리 유틸리티 (memory_manager.h/memory_manager.c)

```c
// src/utils/memory_manager.h
#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stddef.h>
#include "../core/constants.h"

// 2차원 배열 할당/해제
double** allocate_2d_double(int rows, int cols);
int** allocate_2d_int(int rows, int cols);
void deallocate_2d_double(double **array, int rows);
void deallocate_2d_int(int **array, int rows);

// 메모리 사용량 추적
size_t get_memory_usage(void);
void reset_memory_tracking(void);
void print_memory_stats(void);

#endif
```

```c
// src/utils/memory_manager.c
#include "memory_manager.h"
#include <stdlib.h>
#include <stdio.h>

static size_t total_allocated = 0;
static size_t current_allocated = 0;
static int allocation_count = 0;

double** allocate_2d_double(int rows, int cols) {
    if (rows <= 0 || cols <= 0) {
        return NULL;
    }

    double **array = malloc(rows * sizeof(double*));
    if (!array) {
        return NULL;
    }

    for (int i = 0; i < rows; i++) {
        array[i] = malloc(cols * sizeof(double));
        if (!array[i]) {
            // 부분적으로 할당된 메모리 해제
            for (int j = 0; j < i; j++) {
                free(array[j]);
            }
            free(array);
            return NULL;
        }
    }

    // 메모리 사용량 추적
    size_t allocated_size = rows * sizeof(double*) + rows * cols * sizeof(double);
    total_allocated += allocated_size;
    current_allocated += allocated_size;
    allocation_count++;

    return array;
}

int** allocate_2d_int(int rows, int cols) {
    if (rows <= 0 || cols <= 0) {
        return NULL;
    }

    int **array = malloc(rows * sizeof(int*));
    if (!array) {
        return NULL;
    }

    for (int i = 0; i < rows; i++) {
        array[i] = malloc(cols * sizeof(int));
        if (!array[i]) {
            for (int j = 0; j < i; j++) {
                free(array[j]);
            }
            free(array);
            return NULL;
        }
    }

    size_t allocated_size = rows * sizeof(int*) + rows * cols * sizeof(int);
    total_allocated += allocated_size;
    current_allocated += allocated_size;
    allocation_count++;

    return array;
}

void deallocate_2d_double(double **array, int rows) {
    if (!array) return;

    for (int i = 0; i < rows; i++) {
        if (array[i]) {
            free(array[i]);
        }
    }
    free(array);

    size_t deallocated_size = rows * sizeof(double*) + rows * rows * sizeof(double);
    current_allocated -= deallocated_size;
}

void deallocate_2d_int(int **array, int rows) {
    if (!array) return;

    for (int i = 0; i < rows; i++) {
        if (array[i]) {
            free(array[i]);
        }
    }
    free(array);

    size_t deallocated_size = rows * sizeof(int*) + rows * rows * sizeof(int);
    current_allocated -= deallocated_size;
}

size_t get_memory_usage(void) {
    return current_allocated;
}

void reset_memory_tracking(void) {
    total_allocated = 0;
    current_allocated = 0;
    allocation_count = 0;
}

void print_memory_stats(void) {
    printf("=== 메모리 사용량 통계 ===\n");
    printf("총 할당량: %zu bytes\n", total_allocated);
    printf("현재 사용량: %zu bytes\n", current_allocated);
    printf("할당 횟수: %d\n", allocation_count);
    printf("\n");
}
```

## 3. 빌드 시스템

### 3.1 Makefile
```makefile
# Makefile
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -g
INCLUDES = -Isrc
SRCDIR = src
OBJDIR = build
TESTDIR = tests
EXAMPLEDIR = examples

# 소스 파일들
CORE_SOURCES = $(SRCDIR)/core/floyd_warshall.c
DATA_SOURCES = $(SRCDIR)/data_structures/graph.c
UTIL_SOURCES = $(SRCDIR)/utils/memory_manager.c $(SRCDIR)/utils/file_io.c
MAIN_SOURCE = $(SRCDIR)/main.c

SOURCES = $(CORE_SOURCES) $(DATA_SOURCES) $(UTIL_SOURCES)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
MAIN_OBJECT = $(MAIN_SOURCE:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# 실행 파일
TARGET = floyd_warshall
TEST_TARGET = test_floyd_warshall

# 기본 타겟
all: $(TARGET)

# 메인 실행 파일 빌드
$(TARGET): $(OBJECTS) $(MAIN_OBJECT) | $(OBJDIR)
	$(CC) $(OBJECTS) $(MAIN_OBJECT) -o $@ -lm

# 오브젝트 파일 빌드
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# 빌드 디렉토리 생성
$(OBJDIR):
	mkdir -p $(OBJDIR)
	mkdir -p $(OBJDIR)/core
	mkdir -p $(OBJDIR)/data_structures
	mkdir -p $(OBJDIR)/utils

# 테스트 빌드
test: $(OBJECTS) | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDES) $(TESTDIR)/test_*.c $(OBJECTS) -o $(TEST_TARGET) -lm

# 예제 빌드
examples: $(OBJECTS) | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDES) $(EXAMPLEDIR)/simple_graph.c $(OBJECTS) -o simple_graph -lm
	$(CC) $(CFLAGS) $(INCLUDES) $(EXAMPLEDIR)/benchmark.c $(OBJECTS) -o benchmark -lm

# 디버그 빌드
debug: CFLAGS += -DDEBUG -g3
debug: $(TARGET)

# 릴리즈 빌드
release: CFLAGS += -DNDEBUG -O3
release: $(TARGET)

# 정리
clean:
	rm -rf $(OBJDIR)
	rm -f $(TARGET) $(TEST_TARGET) simple_graph benchmark

# 설치
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

# 언인스톨
uninstall:
	rm -f /usr/local/bin/$(TARGET)

# 도움말
help:
	@echo "사용 가능한 타겟:"
	@echo "  all      - 메인 프로그램 빌드 (기본값)"
	@echo "  test     - 테스트 프로그램 빌드"
	@echo "  examples - 예제 프로그램들 빌드"
	@echo "  debug    - 디버그 모드로 빌드"
	@echo "  release  - 릴리즈 모드로 빌드"
	@echo "  clean    - 빌드 파일들 정리"
	@echo "  install  - 시스템에 설치"
	@echo "  help     - 이 도움말 표시"

.PHONY: all test examples debug release clean install uninstall help
```

### 3.2 CMake 빌드 시스템 (선택적)
```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(FloydWarshall C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 컴파일러 옵션
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g -DDEBUG")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -O3 -DNDEBUG")
endif()

# 헤더 파일 디렉토리
include_directories(src)

# 소스 파일들
set(CORE_SOURCES
    src/core/floyd_warshall.c
)

set(DATA_SOURCES
    src/data_structures/graph.c
)

set(UTIL_SOURCES
    src/utils/memory_manager.c
    src/utils/file_io.c
)

set(SOURCES ${CORE_SOURCES} ${DATA_SOURCES} ${UTIL_SOURCES})

# 메인 실행 파일
add_executable(floyd_warshall src/main.c ${SOURCES})
target_link_libraries(floyd_warshall m)

# 테스트 실행 파일
add_executable(test_floyd_warshall
    tests/test_floyd_warshall.c
    tests/test_graph.c
    tests/test_utils.c
    ${SOURCES}
)
target_link_libraries(test_floyd_warshall m)

# 예제들
add_executable(simple_graph examples/simple_graph.c ${SOURCES})
target_link_libraries(simple_graph m)

add_executable(benchmark examples/benchmark.c ${SOURCES})
target_link_libraries(benchmark m)
```

## 4. 테스트 전략

### 4.1 단위 테스트 프레임워크
```c
// tests/test_framework.h
#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdio.h>
#include <stdbool.h>

typedef struct {
    int total_tests;
    int passed_tests;
    int failed_tests;
} TestResult;

extern TestResult global_test_result;

#define ASSERT(condition, message) do { \
    global_test_result.total_tests++; \
    if (condition) { \
        global_test_result.passed_tests++; \
        printf("✓ PASS: %s\n", message); \
    } else { \
        global_test_result.failed_tests++; \
        printf("✗ FAIL: %s\n", message); \
    } \
} while(0)

#define ASSERT_EQUAL(expected, actual, message) do { \
    global_test_result.total_tests++; \
    if ((expected) == (actual)) { \
        global_test_result.passed_tests++; \
        printf("✓ PASS: %s\n", message); \
    } else { \
        global_test_result.failed_tests++; \
        printf("✗ FAIL: %s (expected: %d, actual: %d)\n", \
               message, (int)(expected), (int)(actual)); \
    } \
} while(0)

#define ASSERT_DOUBLE_EQUAL(expected, actual, tolerance, message) do { \
    global_test_result.total_tests++; \
    if (fabs((expected) - (actual)) < (tolerance)) { \
        global_test_result.passed_tests++; \
        printf("✓ PASS: %s\n", message); \
    } else { \
        global_test_result.failed_tests++; \
        printf("✗ FAIL: %s (expected: %.6f, actual: %.6f)\n", \
               message, (expected), (actual)); \
    } \
} while(0)

void test_summary(void);

#endif
```

### 4.2 그래프 테스트
```c
// tests/test_graph.c
#include "test_framework.h"
#include "../src/data_structures/graph.h"
#include <math.h>

TestResult global_test_result = {0, 0, 0};

void test_graph_creation(void) {
    printf("\n=== 그래프 생성 테스트 ===\n");

    // 정상적인 그래프 생성
    Graph *graph = graph_create(5);
    ASSERT(graph != NULL, "정상적인 그래프 생성");
    ASSERT_EQUAL(5, graph->vertices, "정점 수 확인");
    ASSERT(graph->is_initialized, "초기화 상태 확인");

    // 대각선 원소가 0인지 확인
    for (int i = 0; i < 5; i++) {
        ASSERT_DOUBLE_EQUAL(0.0, graph->distance[i][i], EPSILON, "대각선 원소 = 0");
    }

    // 비대각선 원소가 무한대인지 확인
    ASSERT_DOUBLE_EQUAL(INFINITY, graph->distance[0][1], EPSILON, "비대각선 원소 = INF");

    graph_destroy(graph);

    // 잘못된 입력에 대한 테스트
    ASSERT(graph_create(0) == NULL, "0개 정점 그래프 생성 실패");
    ASSERT(graph_create(-1) == NULL, "음수 정점 그래프 생성 실패");
    ASSERT(graph_create(MAX_VERTICES + 1) == NULL, "최대 크기 초과 그래프 생성 실패");
}

void test_graph_edge_operations(void) {
    printf("\n=== 간선 연산 테스트 ===\n");

    Graph *graph = graph_create(4);
    ASSERT(graph != NULL, "테스트용 그래프 생성");

    // 간선 추가
    ASSERT_EQUAL(SUCCESS, graph_add_edge(graph, 0, 1, 5.0), "간선 (0,1) 추가");
    ASSERT_EQUAL(SUCCESS, graph_add_edge(graph, 1, 2, 3.0), "간선 (1,2) 추가");
    ASSERT_EQUAL(SUCCESS, graph_add_edge(graph, 2, 3, 2.0), "간선 (2,3) 추가");

    // 간선 가중치 확인
    ASSERT_DOUBLE_EQUAL(5.0, graph_get_edge(graph, 0, 1), EPSILON, "간선 (0,1) 가중치");
    ASSERT_DOUBLE_EQUAL(3.0, graph_get_edge(graph, 1, 2), EPSILON, "간선 (1,2) 가중치");

    // 존재하지 않는 간선
    ASSERT_DOUBLE_EQUAL(INFINITY, graph_get_edge(graph, 0, 3), EPSILON, "존재하지 않는 간선");

    // 잘못된 간선 추가
    ASSERT_EQUAL(ERROR_INVALID_INPUT, graph_add_edge(graph, -1, 0, 1.0), "잘못된 시작 정점");
    ASSERT_EQUAL(ERROR_INVALID_INPUT, graph_add_edge(graph, 0, 5, 1.0), "잘못된 끝 정점");

    graph_destroy(graph);
}

void run_graph_tests(void) {
    printf("그래프 자료구조 테스트 시작...\n");
    test_graph_creation();
    test_graph_edge_operations();
}
```

### 4.3 Floyd-Warshall 알고리즘 테스트
```c
// tests/test_floyd_warshall.c
#include "test_framework.h"
#include "../src/core/floyd_warshall.h"
#include <math.h>

void test_simple_graph(void) {
    printf("\n=== 간단한 그래프 테스트 ===\n");

    // 3x3 그래프 생성
    Graph *graph = graph_create(3);
    graph_add_edge(graph, 0, 1, 4.0);
    graph_add_edge(graph, 0, 2, 2.0);
    graph_add_edge(graph, 1, 2, 1.0);

    FloydWarshallResult result = floyd_warshall_execute(graph);

    ASSERT(result.success, "알고리즘 실행 성공");
    ASSERT(!result.has_negative_cycle, "음의 사이클 없음");

    // 최단 거리 확인
    ASSERT_DOUBLE_EQUAL(0.0, floyd_warshall_get_distance(graph, 0, 0), EPSILON, "거리 (0,0)");
    ASSERT_DOUBLE_EQUAL(3.0, floyd_warshall_get_distance(graph, 0, 1), EPSILON, "거리 (0,1)");
    ASSERT_DOUBLE_EQUAL(2.0, floyd_warshall_get_distance(graph, 0, 2), EPSILON, "거리 (0,2)");

    graph_destroy(graph);
}

void test_negative_cycle_detection(void) {
    printf("\n=== 음의 사이클 검출 테스트 ===\n");

    // 음의 사이클이 있는 그래프
    Graph *graph = graph_create(3);
    graph_add_edge(graph, 0, 1, 1.0);
    graph_add_edge(graph, 1, 2, -3.0);
    graph_add_edge(graph, 2, 0, 1.0);

    FloydWarshallResult result = floyd_warshall_execute(graph);

    ASSERT(result.success, "알고리즘 실행 성공");
    ASSERT(result.has_negative_cycle, "음의 사이클 검출");

    graph_destroy(graph);
}

void test_path_reconstruction(void) {
    printf("\n=== 경로 복원 테스트 ===\n");

    Graph *graph = graph_create(4);
    graph_add_edge(graph, 0, 1, 5.0);
    graph_add_edge(graph, 0, 3, 10.0);
    graph_add_edge(graph, 1, 2, 3.0);
    graph_add_edge(graph, 2, 3, 1.0);

    floyd_warshall_execute(graph);

    int path_length;
    int *path = floyd_warshall_get_path(graph, 0, 3, &path_length);

    ASSERT(path != NULL, "경로 복원 성공");
    ASSERT_EQUAL(4, path_length, "경로 길이 확인");

    if (path) {
        ASSERT_EQUAL(0, path[0], "경로 시작점");
        ASSERT_EQUAL(1, path[1], "경로 중간점 1");
        ASSERT_EQUAL(2, path[2], "경로 중간점 2");
        ASSERT_EQUAL(3, path[3], "경로 끝점");
        free(path);
    }

    graph_destroy(graph);
}

void run_floyd_warshall_tests(void) {
    printf("Floyd-Warshall 알고리즘 테스트 시작...\n");
    test_simple_graph();
    test_negative_cycle_detection();
    test_path_reconstruction();
}
```

## 5. 성능 최적화

### 5.1 벤치마크 프로그램
```c
// examples/benchmark.c
#include "../src/core/floyd_warshall.h"
#include "../src/utils/memory_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

Graph* create_random_graph(int vertices, double density) {
    Graph *graph = graph_create(vertices);
    if (!graph) return NULL;

    srand(time(NULL));

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

void benchmark_performance(void) {
    int sizes[] = {10, 25, 50, 100, 200};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    printf("=== 성능 벤치마크 ===\n");
    printf("그래프 크기\t실행 시간\t메모리 사용량\n");

    for (int i = 0; i < num_sizes; i++) {
        reset_memory_tracking();

        Graph *graph = create_random_graph(sizes[i], 0.3);
        if (!graph) {
            printf("그래프 생성 실패: %d\n", sizes[i]);
            continue;
        }

        clock_t start = clock();
        FloydWarshallResult result = floyd_warshall_execute(graph);
        clock_t end = clock();

        double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
        size_t memory_used = get_memory_usage();

        printf("%d\t\t%.6f초\t%zu bytes\n",
               sizes[i], time_taken, memory_used);

        graph_destroy(graph);
    }
}

int main(void) {
    benchmark_performance();
    return 0;
}
```

### 5.2 캐시 최적화
```c
// src/core/floyd_warshall_optimized.c
void floyd_warshall_cache_optimized(Graph *graph) {
    int n = graph->vertices;
    double **dist = graph->distance;

    // 블록 크기를 캐시 라인 크기에 맞춤
    const int BLOCK_SIZE = 64;

    for (int k = 0; k < n; k++) {
        for (int i = 0; i < n; i += BLOCK_SIZE) {
            int i_end = (i + BLOCK_SIZE < n) ? i + BLOCK_SIZE : n;

            for (int j = 0; j < n; j += BLOCK_SIZE) {
                int j_end = (j + BLOCK_SIZE < n) ? j + BLOCK_SIZE : n;

                // 블록 내에서 Floyd-Warshall 수행
                for (int ii = i; ii < i_end; ii++) {
                    for (int jj = j; jj < j_end; jj++) {
                        if (dist[ii][k] + dist[k][jj] < dist[ii][jj]) {
                            dist[ii][jj] = dist[ii][k] + dist[k][jj];
                        }
                    }
                }
            }
        }
    }
}
```

## 6. 사용법 가이드

### 6.1 기본 사용법
```c
// examples/simple_graph.c
#include "../src/core/floyd_warshall.h"
#include <stdio.h>

int main(void) {
    // 1. 그래프 생성
    Graph *graph = graph_create(4);
    if (!graph) {
        printf("그래프 생성 실패\n");
        return 1;
    }

    // 2. 간선 추가
    graph_add_edge(graph, 0, 1, 5.0);
    graph_add_edge(graph, 0, 3, 10.0);
    graph_add_edge(graph, 1, 2, 3.0);
    graph_add_edge(graph, 2, 3, 1.0);

    // 3. Floyd-Warshall 알고리즘 실행
    FloydWarshallResult result = floyd_warshall_execute(graph);

    // 4. 결과 확인
    if (result.success) {
        printf("알고리즘 실행 성공!\n");
        floyd_warshall_print_result(&result);
        floyd_warshall_print_distances(graph);

        // 5. 특정 경로 조회
        printf("정점 0에서 3까지의 최단 거리: %.2f\n",
               floyd_warshall_get_distance(graph, 0, 3));

        // 6. 경로 복원
        int path_length;
        int *path = floyd_warshall_get_path(graph, 0, 3, &path_length);
        if (path) {
            printf("최단 경로: ");
            for (int i = 0; i < path_length; i++) {
                printf("%d", path[i]);
                if (i < path_length - 1) printf(" -> ");
            }
            printf("\n");
            free(path);
        }
    } else {
        printf("알고리즘 실행 실패\n");
    }

    // 7. 메모리 정리
    graph_destroy(graph);
    return 0;
}
```

### 6.2 명령행 인터페이스
```c
// src/main.c
#include "core/floyd_warshall.h"
#include "utils/file_io.h"
#include <stdio.h>
#include <string.h>

void print_usage(const char *program_name) {
    printf("사용법: %s [옵션] <그래프파일>\n", program_name);
    printf("옵션:\n");
    printf("  -h, --help     이 도움말 표시\n");
    printf("  -v, --verbose  상세한 출력\n");
    printf("  -o <파일>      결과를 파일로 출력\n");
    printf("  -p <시작> <끝> 특정 경로 표시\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    bool verbose = false;
    char *input_file = NULL;
    char *output_file = NULL;
    int path_start = -1, path_end = -1;

    // 명령행 인수 파싱
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            verbose = true;
        } else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            output_file = argv[++i];
        } else if (strcmp(argv[i], "-p") == 0 && i + 2 < argc) {
            path_start = atoi(argv[++i]);
            path_end = atoi(argv[++i]);
        } else if (input_file == NULL) {
            input_file = argv[i];
        }
    }

    if (!input_file) {
        printf("오류: 입력 파일을 지정해주세요.\n");
        return 1;
    }

    // 그래프 로드
    Graph *graph = load_graph_from_file(input_file);
    if (!graph) {
        printf("오류: 그래프 파일을 읽을 수 없습니다: %s\n", input_file);
        return 1;
    }

    if (verbose) {
        printf("그래프 로드 완료: %d개 정점\n", graph->vertices);
    }

    // Floyd-Warshall 실행
    FloydWarshallResult result = floyd_warshall_execute(graph);

    if (result.success) {
        if (verbose) {
            floyd_warshall_print_result(&result);
        }

        if (result.has_negative_cycle) {
            printf("경고: 음의 사이클이 검출되었습니다.\n");
        } else {
            if (verbose) {
                floyd_warshall_print_distances(graph);
            }

            // 특정 경로 출력
            if (path_start >= 0 && path_end >= 0) {
                double distance = floyd_warshall_get_distance(graph, path_start, path_end);
                printf("최단 거리 (%d -> %d): %.2f\n", path_start, path_end, distance);

                int path_length;
                int *path = floyd_warshall_get_path(graph, path_start, path_end, &path_length);
                if (path) {
                    printf("경로: ");
                    for (int i = 0; i < path_length; i++) {
                        printf("%d", path[i]);
                        if (i < path_length - 1) printf(" -> ");
                    }
                    printf("\n");
                    free(path);
                }
            }

            // 결과 파일 저장
            if (output_file) {
                if (save_result_to_file(graph, output_file)) {
                    printf("결과가 저장되었습니다: %s\n", output_file);
                } else {
                    printf("오류: 결과 파일을 저장할 수 없습니다: %s\n", output_file);
                }
            }
        }
    } else {
        printf("오류: 알고리즘 실행 실패\n");
        graph_destroy(graph);
        return 1;
    }

    graph_destroy(graph);
    return 0;
}
```

이 구현 가이드를 따라 Floyd-Warshall 알고리즘을 체계적이고 효율적으로 구현할 수 있습니다. 각 모듈은 독립적으로 테스트 가능하며, 확장성과 유지보수성을 고려하여 설계되었습니다.