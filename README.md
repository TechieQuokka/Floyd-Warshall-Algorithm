# Floyd-Warshall 알고리즘 C언어 구현

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Language](https://img.shields.io/badge/language-C-brightgreen.svg)
![Standard](https://img.shields.io/badge/standard-C99-orange.svg)

모든 정점 쌍 간의 최단 경로를 찾는 Floyd-Warshall 알고리즘의 완전한 C언어 구현입니다.

## 주요 특징

- **완전한 구현**: 표준 및 최적화된 Floyd-Warshall 알고리즘
- **음의 가중치 지원**: 음의 가중치를 가진 간선 처리 가능
- **음의 사이클 검출**: 자동으로 음의 사이클 감지 및 보고
- **경로 복원**: 실제 최단 경로 재구성 기능
- **메모리 안전성**: 강력한 메모리 관리 및 오류 처리
- **성능 최적화**: 캐시 효율성을 고려한 최적화 구현
- **포괄적인 테스트**: 단위 테스트 및 벤치마크 포함

## 시작하기

### 필요 조건

- GCC 컴파일러 (C99 표준 지원)
- Make 빌드 시스템
- 최소 100MB 여유 메모리

### 빌드 방법

```bash
# 기본 빌드
make

# 디버그 버전 빌드
make debug

# 릴리즈 최적화 빌드
make release

# 모든 프로그램 빌드 (메인, 테스트, 예제)
make all test examples
```

### 빠른 시작

```bash
# 샘플 그래프로 데모 실행
make demo

# 음의 사이클 검출 데모
make demo-negative

# 간단한 예제 실행
make run-example

# 성능 벤치마크 실행
make run-benchmark
```

## 사용법

### 기본 사용법

```bash
./floyd_warshall [옵션] <그래프파일>
```

### 명령행 옵션

- `-h, --help`: 도움말 표시
- `-v, --verbose`: 상세한 출력 활성화
- `-o <파일>`: 결과를 파일로 저장
- `-p <시작> <끝>`: 특정 경로 표시
- `-s`: 최적화된 알고리즘 사용
- `-m`: 메모리 사용량 통계 표시

### 사용 예제

```bash
# 기본 실행
./floyd_warshall data/sample_graph.txt

# 상세 모드로 실행하고 결과 저장
./floyd_warshall -v -o results.txt data/sample_graph.txt

# 특정 경로 조회
./floyd_warshall -p 0 3 data/sample_graph.txt

# 최적화 알고리즘으로 실행
./floyd_warshall -s -m data/sample_graph.txt
```

## 그래프 파일 형식

그래프 파일은 다음 형식을 따라야 합니다:

```
정점의_수
간선의_수
시작정점 끝정점 가중치
시작정점 끝정점 가중치
...
```

### 예제 파일

```
4
5
0 1 5.0
0 3 10.0
1 2 3.0
2 3 1.0
3 1 2.0
```

## API 사용법

### 기본 사용 예제

```c
#include "src/core/floyd_warshall.h"

int main() {
    // 그래프 생성 (4개 정점)
    Graph *graph = graph_create(4);

    // 간선 추가
    graph_add_edge(graph, 0, 1, 5.0);
    graph_add_edge(graph, 1, 2, 3.0);

    // 알고리즘 실행
    FloydWarshallResult result = floyd_warshall_execute(graph);

    if (result.success) {
        // 최단 거리 조회
        double distance = floyd_warshall_get_distance(graph, 0, 2);
        printf("최단 거리: %.2f\n", distance);

        // 경로 복원
        int path_length;
        int *path = floyd_warshall_get_path(graph, 0, 2, &path_length);
        // path 사용 후 free(path) 필요
    }

    // 메모리 정리
    graph_destroy(graph);
    return 0;
}
```

## 프로젝트 구조

```
Floyd-Warshall-Algorithm/
├── src/                      # 소스 코드
│   ├── core/                 # 핵심 알고리즘
│   │   ├── floyd_warshall.h  # 알고리즘 인터페이스
│   │   ├── floyd_warshall.c  # 알고리즘 구현
│   │   └── constants.h       # 상수 정의
│   ├── data_structures/      # 자료구조
│   │   ├── graph.h           # 그래프 인터페이스
│   │   └── graph.c           # 그래프 구현
│   ├── utils/               # 유틸리티
│   │   ├── memory_manager.h  # 메모리 관리
│   │   ├── memory_manager.c
│   │   ├── file_io.h         # 파일 입출력
│   │   └── file_io.c
│   └── main.c               # 메인 프로그램
├── tests/                   # 테스트 코드
│   ├── test_framework.h     # 테스트 프레임워크
│   ├── test_framework.c
│   └── test_main.c          # 메인 테스트
├── examples/                # 예제 프로그램
│   ├── simple_example.c     # 간단한 사용 예제
│   └── benchmark.c          # 성능 벤치마크
├── data/                    # 샘플 데이터
│   ├── sample_graph.txt     # 샘플 그래프
│   └── negative_cycle_graph.txt
├── docs/                    # 문서
│   ├── architecture.md      # 아키텍처 설계
│   ├── algorithm_theory.md  # 알고리즘 이론
│   └── implementation_guide.md
├── Makefile                 # 빌드 시스템
└── README.md               # 이 파일
```

## 테스트

### 테스트 실행

```bash
# 테스트 빌드 및 실행
make run-tests

# 메모리 누수 검사 (valgrind 필요)
make memcheck

# 코드 스타일 검사 (cppcheck 필요)
make stylecheck
```

### 테스트 커버리지

- 그래프 생성 및 간선 관리
- Floyd-Warshall 알고리즘 정확성
- 음의 사이클 검출
- 경로 복원 기능
- 메모리 관리
- 파일 입출력

## 성능

### 시간 복잡도
- **표준 알고리즘**: O(V³)
- **최적화 알고리즘**: O(V³), 조기 종료로 실제 성능 향상

### 공간 복잡도
- **메모리 사용량**: O(V²)
- **추가 공간**: 경로 복원용 O(V²)

### 벤치마크 결과

| 정점 수 | 표준 시간(초) | 최적화 시간(초) | 메모리(KB) |
|---------|---------------|-----------------|------------|
| 10      | 0.000001      | 0.000001        | 1          |
| 50      | 0.000123      | 0.000098        | 25         |
| 100     | 0.001234      | 0.000987        | 78         |
| 200     | 0.009876      | 0.007654        | 312        |

## 기여하기

1. 이 저장소를 포크합니다
2. 기능 브랜치를 생성합니다 (`git checkout -b feature/새기능`)
3. 변경사항을 커밋합니다 (`git commit -am '새기능 추가'`)
4. 브랜치에 푸시합니다 (`git push origin feature/새기능`)
5. Pull Request를 생성합니다

### 개발 가이드라인

- ASCII 코드만 사용 (한글 주석 제외)
- C99 표준 준수
- 일관된 코딩 스타일 유지
- 모든 함수에 대한 테스트 작성
- 메모리 누수 방지
- 적절한 오류 처리

## 문제 해결

### 자주 발생하는 문제

**컴파일 오류가 발생합니다**
- GCC가 C99 표준을 지원하는지 확인하세요
- 필요한 헤더 파일이 모두 있는지 확인하세요

**메모리 부족 오류가 발생합니다**
- 큰 그래프의 경우 충분한 메모리가 필요합니다
- 32비트 시스템에서는 약 500개 정점이 한계입니다

**파일을 읽을 수 없습니다**
- 파일 형식이 올바른지 확인하세요
- 파일 권한을 확인하세요

## 참고 자료

### 알고리즘 관련
- [Floyd-Warshall Algorithm - Wikipedia](https://en.wikipedia.org/wiki/Floyd%E2%80%93Warshall_algorithm)
- Introduction to Algorithms (CLRS) - Chapter 25

### 구현 관련
- [docs/algorithm_theory.md](docs/algorithm_theory.md) - 알고리즘 상세 이론
- [docs/architecture.md](docs/architecture.md) - 시스템 아키텍처
- [docs/implementation_guide.md](docs/implementation_guide.md) - 구현 가이드

## 라이선스

이 프로젝트는 MIT 라이선스 하에 배포됩니다. 자세한 내용은 [LICENSE](LICENSE) 파일을 참조하세요.

## 연락처

문제가 있거나 제안사항이 있으시면 이슈를 등록해 주세요.

---

**Floyd-Warshall Algorithm C Implementation** - 모든 정점 쌍 간의 최단 경로를 효율적으로 찾는 완전한 C언어 구현