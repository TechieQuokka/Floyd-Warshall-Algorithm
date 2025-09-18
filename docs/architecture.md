# Floyd-Warshall 알고리즘 아키텍처 설계 문서

## 1. 개요

### 1.1 프로젝트 목적
Floyd-Warshall 알고리즘을 C언어로 구현하여 모든 정점 쌍 간의 최단 경로를 찾는 효율적인 그래프 처리 시스템을 제공합니다.

### 1.2 설계 목표
- **성능**: O(V³) 시간 복잡도로 최적화된 구현
- **메모리 효율성**: 동적 메모리 할당을 통한 유연한 그래프 크기 지원
- **모듈성**: 재사용 가능한 컴포넌트 구조
- **확장성**: 다양한 그래프 타입과 가중치 지원
- **안정성**: 강건한 오류 처리 및 메모리 관리

## 2. 시스템 아키텍처

### 2.1 전체 구조
```
Floyd-Warshall-Algorithm/
├── src/
│   ├── core/
│   │   ├── floyd_warshall.h      # 알고리즘 핵심 인터페이스
│   │   ├── floyd_warshall.c      # 알고리즘 구현
│   │   └── constants.h           # 상수 정의
│   ├── data_structures/
│   │   ├── graph.h               # 그래프 자료구조 인터페이스
│   │   ├── graph.c               # 그래프 구현
│   │   └── matrix.h              # 행렬 연산 인터페이스
│   ├── utils/
│   │   ├── memory_manager.h      # 메모리 관리 유틸리티
│   │   ├── memory_manager.c
│   │   ├── file_io.h             # 파일 입출력 처리
│   │   ├── file_io.c
│   │   └── logger.h              # 로깅 시스템
│   └── main.c                    # 메인 애플리케이션
├── tests/
│   ├── test_floyd_warshall.c     # 알고리즘 단위 테스트
│   ├── test_graph.c              # 그래프 테스트
│   └── test_utils.c              # 유틸리티 테스트
├── examples/
│   ├── simple_graph.c            # 기본 사용 예제
│   └── benchmark.c               # 성능 벤치마크
├── data/
│   ├── sample_graphs/            # 샘플 그래프 데이터
│   └── test_cases/               # 테스트 케이스
├── docs/
│   ├── architecture.md           # 이 문서
│   ├── algorithm_theory.md       # 알고리즘 이론
│   └── api_reference.md          # API 참조
├── build/                        # 빌드 디렉토리
├── Makefile                      # 빌드 설정
└── README.md                     # 프로젝트 소개
```

### 2.2 컴포넌트 다이어그램
```
┌─────────────────────────────────────────────────────────┐
│                    Main Application                     │
└─────────────────────┬───────────────────────────────────┘
                      │
┌─────────────────────▼───────────────────────────────────┐
│                Floyd-Warshall Core                     │
│  ┌─────────────────┐  ┌─────────────────┐              │
│  │ Algorithm Engine│  │ Path Reconstruction│           │
│  └─────────────────┘  └─────────────────┘              │
└─────────────────────┬───────────────────────────────────┘
                      │
┌─────────────────────▼───────────────────────────────────┐
│                Data Structures                         │
│  ┌─────────────────┐  ┌─────────────────┐              │
│  │  Graph Manager  │  │ Distance Matrix │              │
│  └─────────────────┘  └─────────────────┘              │
└─────────────────────┬───────────────────────────────────┘
                      │
┌─────────────────────▼───────────────────────────────────┐
│                   Utilities                            │
│  ┌────────────┐ ┌────────────┐ ┌────────────┐          │
│  │Memory Mgmt │ │  File I/O  │ │   Logger   │          │
│  └────────────┘ └────────────┘ └────────────┘          │
└─────────────────────────────────────────────────────────┘
```

## 3. 핵심 모듈 설계

### 3.1 Floyd-Warshall Core 모듈

#### 구조체 정의
```c
// 그래프 표현을 위한 구조체
typedef struct {
    int vertices;           // 정점 수
    double **distance;      // 거리 행렬
    int **next;            // 경로 복원을 위한 행렬
    bool *visited;         // 방문 여부 (옵션)
} Graph;

// 알고리즘 실행 결과
typedef struct {
    bool success;          // 실행 성공 여부
    double execution_time; // 실행 시간
    int iterations;        // 반복 횟수
    bool has_negative_cycle; // 음의 사이클 존재 여부
} FloydWarshallResult;
```

#### 핵심 함수 인터페이스
```c
// 알고리즘 실행
FloydWarshallResult floyd_warshall(Graph *graph);

// 경로 복원
int* reconstruct_path(Graph *graph, int start, int end, int *path_length);

// 음의 사이클 검출
bool detect_negative_cycle(Graph *graph);

// 최단 거리 조회
double get_shortest_distance(Graph *graph, int start, int end);
```

### 3.2 그래프 자료구조 모듈

#### 그래프 관리 함수
```c
// 그래프 생성 및 초기화
Graph* create_graph(int vertices);

// 간선 추가
bool add_edge(Graph *graph, int from, int to, double weight);

// 그래프 메모리 해제
void destroy_graph(Graph *graph);

// 그래프 복사
Graph* copy_graph(const Graph *original);

// 그래프 유효성 검사
bool validate_graph(const Graph *graph);
```

### 3.3 메모리 관리 모듈

#### 메모리 할당 및 해제
```c
// 2차원 배열 동적 할당
double** allocate_2d_double(int rows, int cols);
int** allocate_2d_int(int rows, int cols);

// 2차원 배열 메모리 해제
void deallocate_2d_double(double **array, int rows);
void deallocate_2d_int(int **array, int rows);

// 메모리 사용량 추적
size_t get_memory_usage(void);
```

### 3.4 파일 입출력 모듈

#### 그래프 파일 처리
```c
// 파일에서 그래프 로드
Graph* load_graph_from_file(const char *filename);

// 그래프를 파일로 저장
bool save_graph_to_file(const Graph *graph, const char *filename);

// 결과 출력
bool save_result_to_file(const Graph *graph, const char *filename);
```

## 4. 데이터 흐름

### 4.1 알고리즘 실행 흐름
```
1. 그래프 초기화
   ├── 메모리 할당
   ├── 거리 행렬 초기화 (무한대)
   └── 대각선 원소를 0으로 설정

2. Floyd-Warshall 알고리즘 실행
   ├── 3중 반복문 (k, i, j)
   ├── 거리 갱신: dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j])
   └── 경로 정보 갱신

3. 결과 처리
   ├── 음의 사이클 검출
   ├── 최종 거리 행렬 생성
   └── 경로 복원 정보 준비

4. 메모리 정리
   └── 동적 할당된 메모리 해제
```

### 4.2 메모리 레이아웃
```
Graph Structure:
┌─────────────────┐
│   vertices (4)  │ ← 정점 수
├─────────────────┤
│  distance (**) ─┼─→ ┌─────┬─────┬─────┬─────┐
├─────────────────┤   │ 0.0 │ 3.0 │ INF │ 7.0 │
│    next (**)  ──┼─→ ├─────┼─────┼─────┼─────┤
├─────────────────┤   │ 8.0 │ 0.0 │ 2.0 │ INF │
│   visited (*)  ─┼─→ ├─────┼─────┼─────┼─────┤
└─────────────────┘   │ 5.0 │ INF │ 0.0 │ 1.0 │
                      ├─────┼─────┼─────┼─────┤
                      │ 2.0 │ INF │ INF │ 0.0 │
                      └─────┴─────┴─────┴─────┘
```

## 5. 성능 최적화 전략

### 5.1 알고리즘 최적화
- **캐시 지역성**: 행렬 접근 패턴 최적화
- **분할 정복**: 큰 그래프에 대한 블록 단위 처리
- **조기 종료**: 변경이 없는 경우 반복 중단
- **메모리 정렬**: 캐시 라인에 맞춘 데이터 정렬

### 5.2 메모리 최적화
- **지연 할당**: 필요할 때만 메모리 할당
- **메모리 풀**: 빈번한 할당/해제 최소화
- **압축**: 희소 그래프에 대한 압축 표현

### 5.3 병렬화 고려사항
- **OpenMP**: 반복문 병렬화 준비
- **스레드 안전성**: 공유 데이터 보호
- **작업 분할**: 효율적인 작업 단위 설계

## 6. 오류 처리 및 검증

### 6.1 입력 검증
- 그래프 크기 유효성
- 간선 가중치 범위 확인
- 메모리 할당 실패 처리

### 6.2 실행 시 검증
- 음의 사이클 검출
- 오버플로우 방지
- 무한대 값 처리

### 6.3 출력 검증
- 결과 일관성 확인
- 경로 복원 정확성
- 메모리 누수 방지

## 7. 테스트 전략

### 7.1 단위 테스트
- 개별 함수 기능 검증
- 경계값 테스트
- 예외 상황 처리

### 7.2 통합 테스트
- 전체 알고리즘 검증
- 다양한 그래프 타입 테스트
- 성능 벤치마크

### 7.3 검증 데이터셋
- 알려진 해답이 있는 그래프
- 음의 사이클을 포함한 그래프
- 대규모 그래프 데이터

## 8. 확장성 고려사항

### 8.1 향후 개선 방향
- 다양한 거리 함수 지원
- 그래프 시각화 기능
- 웹 인터페이스 추가
- 분산 처리 지원

### 8.2 호환성
- 다양한 컴파일러 지원
- 크로스 플랫폼 호환성
- 표준 C99/C11 준수

이 아키텍처 설계는 Floyd-Warshall 알고리즘의 효율적이고 확장 가능한 C언어 구현을 위한 기반을 제공합니다.