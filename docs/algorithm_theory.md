# Floyd-Warshall 알고리즘: 이론과 구현

## 목차
1. [알고리즘 개요](#1-알고리즘-개요)
2. [수학적 기반](#2-수학적-기반)
3. [알고리즘 원리](#3-알고리즘-원리)
4. [시간 및 공간 복잡도](#4-시간-및-공간-복잡도)
5. [구현 세부사항](#5-구현-세부사항)
6. [최적화 기법](#6-최적화-기법)
7. [응용 분야](#7-응용-분야)
8. [비교 분석](#8-비교-분석)

## 1. 알고리즘 개요

### 1.1 정의
Floyd-Warshall 알고리즘은 가중 그래프에서 모든 정점 쌍 간의 최단 경로를 찾는 동적 계획법(Dynamic Programming) 기반 알고리즘입니다. 1962년 Robert Floyd가 제안하였으며, Stephen Warshall의 이전 연구를 확장한 것입니다.

### 1.2 특징
- **완전성**: 모든 정점 쌍에 대한 최단 경로를 한 번에 계산
- **음의 가중치 지원**: 음의 가중치를 가진 간선도 처리 가능
- **음의 사이클 검출**: 음의 사이클 존재 여부 판별 가능
- **단순성**: 구현이 상대적으로 간단함
- **안정성**: 수치적으로 안정적인 알고리즘

### 1.3 적용 조건
- 방향 그래프 또는 무방향 그래프 모두 적용 가능
- 가중 그래프에서 사용
- 음의 사이클이 없는 그래프에서 정확한 결과 보장
- 정점 수가 중간 규모(일반적으로 1000개 이하)인 그래프에 적합

## 2. 수학적 기반

### 2.1 점화식
Floyd-Warshall 알고리즘의 핵심은 다음 점화식입니다:

```
dist[i][j]^(k) = min(dist[i][j]^(k-1), dist[i][k]^(k-1) + dist[k][j]^(k-1))
```

여기서:
- `dist[i][j]^(k)`: 정점 {1, 2, ..., k}만을 중간 정점으로 사용하여 정점 i에서 j로 가는 최단 경로의 길이
- `k`: 현재 고려하는 중간 정점의 인덱스

### 2.2 초기 조건
```
dist[i][j]^(0) = {
    0           if i = j
    weight(i,j) if (i,j) ∈ E
    ∞           otherwise
}
```

### 2.3 최종 해
모든 k에 대해 반복을 완료한 후, `dist[i][j]^(n)`이 정점 i에서 j로의 최단 경로 길이가 됩니다.

### 2.4 최적 부분 구조
Floyd-Warshall 알고리즘이 동적 계획법을 사용할 수 있는 이유는 최단 경로 문제가 최적 부분 구조(Optimal Substructure)를 가지기 때문입니다:

- 정점 i에서 j로의 최단 경로가 중간 정점 k를 거친다면
- i에서 k로의 부분 경로와 k에서 j로의 부분 경로도 각각 최단 경로여야 함

## 3. 알고리즘 원리

### 3.1 기본 아이디어
1. **단계별 확장**: 각 단계에서 하나의 정점을 중간 정점으로 추가
2. **경로 비교**: 기존 경로와 새로운 중간 정점을 거치는 경로를 비교
3. **최적값 갱신**: 더 짧은 경로로 거리 행렬 갱신

### 3.2 알고리즘 절차

#### 단계 1: 초기화
```c
// 거리 행렬 초기화
for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
        if (i == j) {
            dist[i][j] = 0;
        } else if (간선 (i,j)가 존재) {
            dist[i][j] = weight(i, j);
        } else {
            dist[i][j] = INFINITY;
        }
    }
}
```

#### 단계 2: 동적 계획법 적용
```c
// Floyd-Warshall 핵심 로직
for (int k = 0; k < n; k++) {          // 중간 정점
    for (int i = 0; i < n; i++) {      // 시작 정점
        for (int j = 0; j < n; j++) {  // 끝 정점
            if (dist[i][k] + dist[k][j] < dist[i][j]) {
                dist[i][j] = dist[i][k] + dist[k][j];
                next[i][j] = next[i][k];  // 경로 복원용
            }
        }
    }
}
```

#### 단계 3: 음의 사이클 검출
```c
// 음의 사이클 검출
for (int i = 0; i < n; i++) {
    if (dist[i][i] < 0) {
        // 음의 사이클 존재
        return NEGATIVE_CYCLE_DETECTED;
    }
}
```

### 3.3 경로 복원
실제 최단 경로를 복원하기 위해 `next` 배열을 사용합니다:

```c
void print_path(int **next, int i, int j) {
    if (next[i][j] == -1) {
        printf("경로가 존재하지 않습니다\n");
        return;
    }

    printf("%d", i);
    while (i != j) {
        i = next[i][j];
        printf(" -> %d", i);
    }
    printf("\n");
}
```

## 4. 시간 및 공간 복잡도

### 4.1 시간 복잡도
- **최선/평균/최악의 경우**: O(V³)
  - 3중 반복문으로 인한 고정적인 시간 복잡도
  - 입력 그래프의 특성과 무관하게 일정함

### 4.2 공간 복잡도
- **기본 구현**: O(V²)
  - 거리 행렬을 위한 공간
  - 경로 복원을 위한 추가 행렬 필요시 O(V²) 추가

### 4.3 최적화된 공간 복잡도
```c
// 제자리 업데이트 (In-place update)
// 별도의 임시 배열 없이 기존 배열 직접 수정
for (int k = 0; k < n; k++) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j]);
        }
    }
}
```

## 5. 구현 세부사항

### 5.1 무한대 처리
```c
#define INFINITY 1e9  // 실용적인 무한대 값
#define INF INT_MAX/2 // 오버플로우 방지

// 더 안전한 비교
bool is_reachable(double dist) {
    return dist < INFINITY;
}
```

### 5.2 부동소수점 정밀도
```c
#define EPSILON 1e-9

bool double_equals(double a, double b) {
    return fabs(a - b) < EPSILON;
}

bool is_shorter_path(double new_dist, double old_dist) {
    return new_dist < old_dist - EPSILON;
}
```

### 5.3 메모리 효율적인 구현
```c
// 블록 단위 처리로 캐시 효율성 향상
void floyd_warshall_blocked(double **dist, int n, int block_size) {
    for (int k = 0; k < n; k += block_size) {
        int k_end = min(k + block_size, n);

        for (int i = 0; i < n; i += block_size) {
            int i_end = min(i + block_size, n);

            for (int j = 0; j < n; j += block_size) {
                int j_end = min(j + block_size, n);

                // 블록 단위로 Floyd-Warshall 수행
                for (int kk = k; kk < k_end; kk++) {
                    for (int ii = i; ii < i_end; ii++) {
                        for (int jj = j; jj < j_end; jj++) {
                            double new_dist = dist[ii][kk] + dist[kk][jj];
                            if (new_dist < dist[ii][jj]) {
                                dist[ii][jj] = new_dist;
                            }
                        }
                    }
                }
            }
        }
    }
}
```

## 6. 최적화 기법

### 6.1 조기 종료 최적화
```c
bool floyd_warshall_early_termination(Graph *graph) {
    bool changed = true;

    for (int k = 0; k < graph->vertices && changed; k++) {
        changed = false;

        for (int i = 0; i < graph->vertices; i++) {
            for (int j = 0; j < graph->vertices; j++) {
                double new_dist = graph->distance[i][k] + graph->distance[k][j];

                if (new_dist < graph->distance[i][j]) {
                    graph->distance[i][j] = new_dist;
                    graph->next[i][j] = graph->next[i][k];
                    changed = true;
                }
            }
        }
    }

    return !changed; // 수렴 여부 반환
}
```

### 6.2 희소 그래프 최적화
```c
// 인접 리스트 기반 접근법으로 불필요한 연산 줄이기
typedef struct Edge {
    int to;
    double weight;
    struct Edge *next;
} Edge;

typedef struct {
    int vertices;
    Edge **adj_list;
    double **dist;
} SparseGraph;
```

### 6.3 병렬화
```c
#ifdef _OPENMP
#include <omp.h>

void floyd_warshall_parallel(double **dist, int n) {
    for (int k = 0; k < n; k++) {
        #pragma omp parallel for
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                double new_dist = dist[i][k] + dist[k][j];
                if (new_dist < dist[i][j]) {
                    dist[i][j] = new_dist;
                }
            }
        }
    }
}
#endif
```

## 7. 응용 분야

### 7.1 네트워크 라우팅
- 패킷 라우팅 테이블 생성
- 네트워크 토폴로지 분석
- 대역폭 최적화

### 7.2 교통 시스템
- 도로 네트워크에서의 최단 경로
- 대중교통 경로 계획
- 물류 최적화

### 7.3 게임 AI
- 게임 맵에서의 경로 탐색
- NPC 이동 경로 계산
- 전략 게임의 이동 계획

### 7.4 그래프 이론 연구
- 그래프의 연결성 분석
- 중심성 측정
- 클러스터링 분석

## 8. 비교 분석

### 8.1 다른 최단 경로 알고리즘과의 비교

| 알고리즘 | 시간 복잡도 | 공간 복잡도 | 음의 가중치 | 모든 쌍 | 구현 난이도 |
|---------|------------|-----------|-----------|-------|-----------|
| Floyd-Warshall | O(V³) | O(V²) | ✓ | ✓ | 쉬움 |
| Dijkstra (모든 쌍) | O(V³ log V) | O(V²) | ✗ | ✓ | 중간 |
| Bellman-Ford (모든 쌍) | O(V⁴) | O(V²) | ✓ | ✓ | 중간 |
| Johnson | O(V² log V + VE) | O(V²) | ✓ | ✓ | 어려움 |

### 8.2 장단점 분석

#### 장점
- **구현의 단순성**: 3중 반복문으로 간단히 구현
- **음의 가중치 처리**: 추가적인 전처리 없이 음의 가중치 지원
- **음의 사이클 검출**: 자연스럽게 음의 사이클 검출 가능
- **모든 쌍 최단 경로**: 한 번의 실행으로 모든 정점 쌍의 최단 경로 계산
- **수치 안정성**: 부동소수점 연산에서 비교적 안정적

#### 단점
- **높은 시간 복잡도**: O(V³)로 큰 그래프에서 비효율적
- **고정된 복잡도**: 그래프가 희소해도 시간 복잡도가 줄어들지 않음
- **높은 공간 복잡도**: O(V²)의 메모리 요구사항
- **확장성 한계**: 대규모 그래프에서는 실용적이지 않음

### 8.3 적용 가이드라인

#### Floyd-Warshall 선택 기준
- 정점 수 < 500인 경우
- 모든 정점 쌍의 최단 경로가 필요한 경우
- 음의 가중치가 있는 경우
- 구현의 단순성이 중요한 경우

#### 대안 알고리즘 고려
- **큰 그래프**: Johnson 알고리즘
- **희소 그래프**: 모든 정점에서 Dijkstra 실행
- **음의 가중치 없음**: 모든 정점에서 Dijkstra 실행
- **단일 쌍 최단 경로**: A* 또는 Dijkstra

## 9. 구현 예제 및 테스트

### 9.1 완전한 구현 예제
```c
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <stdbool.h>

#define INFINITY DBL_MAX
#define MAX_VERTICES 1000

typedef struct {
    int vertices;
    double **distance;
    int **next;
} Graph;

Graph* create_graph(int vertices) {
    Graph *graph = malloc(sizeof(Graph));
    graph->vertices = vertices;

    // 거리 행렬 할당
    graph->distance = malloc(vertices * sizeof(double*));
    graph->next = malloc(vertices * sizeof(int*));

    for (int i = 0; i < vertices; i++) {
        graph->distance[i] = malloc(vertices * sizeof(double));
        graph->next[i] = malloc(vertices * sizeof(int));

        for (int j = 0; j < vertices; j++) {
            graph->distance[i][j] = (i == j) ? 0 : INFINITY;
            graph->next[i][j] = -1;
        }
    }

    return graph;
}

bool floyd_warshall(Graph *graph) {
    int n = graph->vertices;

    // Floyd-Warshall 알고리즘 실행
    for (int k = 0; k < n; k++) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (graph->distance[i][k] != INFINITY &&
                    graph->distance[k][j] != INFINITY) {

                    double new_dist = graph->distance[i][k] + graph->distance[k][j];

                    if (new_dist < graph->distance[i][j]) {
                        graph->distance[i][j] = new_dist;
                        graph->next[i][j] = k;
                    }
                }
            }
        }
    }

    // 음의 사이클 검출
    for (int i = 0; i < n; i++) {
        if (graph->distance[i][i] < 0) {
            return false; // 음의 사이클 존재
        }
    }

    return true; // 성공
}
```

### 9.2 성능 테스트 케이스
```c
void performance_test() {
    int sizes[] = {10, 50, 100, 200, 500};
    int num_tests = sizeof(sizes) / sizeof(sizes[0]);

    for (int i = 0; i < num_tests; i++) {
        Graph *graph = create_random_graph(sizes[i], 0.3); // 30% 밀도

        clock_t start = clock();
        floyd_warshall(graph);
        clock_t end = clock();

        double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
        printf("정점 %d개: %.6f초\n", sizes[i], time_taken);

        destroy_graph(graph);
    }
}
```

## 10. 결론

Floyd-Warshall 알고리즘은 모든 정점 쌍 간의 최단 경로를 찾는 강력하고 직관적인 알고리즘입니다. O(V³)의 시간 복잡도로 인해 대규모 그래프에서는 제한이 있지만, 중소 규모 그래프에서는 구현의 단순성과 안정성으로 인해 널리 사용됩니다.

음의 가중치 처리와 음의 사이클 검출 기능, 그리고 모든 정점 쌍의 최단 경로를 한 번에 계산하는 능력은 이 알고리즘의 주요 장점입니다. 적절한 최적화 기법을 적용하면 실제 응용에서도 충분히 효율적으로 사용할 수 있습니다.

이 이론적 기반을 바탕으로 실제 C언어 구현에서는 메모리 관리, 오류 처리, 성능 최적화에 중점을 두어 실용적이고 안정적인 시스템을 구축할 수 있습니다.