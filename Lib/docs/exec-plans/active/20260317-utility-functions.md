# Exec Plan: Lib 유틸리티 함수 추가

- **날짜**: 2026-03-17
- **상태**: active
- **담당 영역**: `Lib/include/utils/`
- **네임스페이스**: `portpolio::lib::utils`

---

## 목표

게임 서버에서 범용으로 쓸 수 있는 유틸리티 헤더를 `Lib/include/utils/` 아래에 추가한다.
서버·클라이언트 공용 코드이므로 외부 의존성 없이 표준 라이브러리만 사용한다.

---

## 범위

### 1. 문자열 유틸리티 (`string_utils.h`)

| 함수 | 설명 |
|------|------|
| `trim(str)` | 앞뒤 공백 제거 (복사 반환) |
| `trim_in_place(str)` | 앞뒤 공백 제거 (in-place) |
| `to_lower(str)` | 소문자 변환 (복사 반환) |
| `to_upper(str)` | 대문자 변환 (복사 반환) |
| `split(str, delim)` | 구분자(`string_view`)로 분리 → `vector<string>` |
| `split(str, delim)` | 구분자(`const char*` / char 배열 포함) 오버로드 |
| `starts_with(str, prefix)` | 접두사 확인 (C++17 호환) |
| `ends_with(str, suffix)` | 접미사 확인 (C++17 호환) |
| `join(range, delim)` | 문자열 range를 구분자로 결합 |
| `join(args..., delim)` | 가변 인자 문자열들을 구분자로 결합 |

**split 오버로드 상세**
- `split(string_view src, string_view delim)` — 다중 문자 구분자
- `split(string_view src, const char* delim)` — C 문자열 / char 배열 구분자 (위로 포워딩)

**join 오버로드 상세**
- `join(Range&& range, string_view delim)` — range/컨테이너를 받아 결합
- `join(string_view delim, Args&&... args)` — 가변 인자 문자열들을 결합

### 2. 컨테이너 헬퍼 (`container_utils.h`)

`std::ranges::views`를 래핑하여 range 기반으로 동작한다.

| 함수 / 뷰 | 설명 |
|-----------|------|
| `contains(range, value)` | range 전체를 순회해 값 존재 여부 확인 (`ranges::find` 기반) |
| `find_if_or(range, pred, default)` | 조건 검색, 없으면 기본값 반환 |
| `erase_if(container, pred)` | 조건 만족 요소 일괄 삭제 (`std::erase_if` 위임) |
| `transform_to_vec(range, func)` | `views::transform` 결과를 `vector`로 수집 |
| `keys(map_range)` | `views::keys` 래퍼 — map/pair range에서 key view 반환 |
| `values(map_range)` | `views::values` 래퍼 — map/pair range에서 value view 반환 |

> `keys` / `values`는 lazy view를 반환한다. 즉시 벡터가 필요하면 `transform_to_vec` 와 조합한다.

### 3. 랜덤 유틸리티 (`random_utils.h`)

#### 3-1. `RandomEngine` — 전용 난수 엔진 래퍼 (값 객체)

```cpp
class RandomEngine {
public:
    explicit RandomEngine();                        // mt19937, random_device 시드
    explicit RandomEngine(uint64_t seed);           // 재현 가능한 시드

    int    rand_int(int min, int max);              // [min, max] 균등 정수
    int    rand_int(IntRange range);
    double rand_float(double min, double max);      // [min, max) 균등 실수
    double rand_float(FloatRange range);
    bool   rand_bool(double probability);           // 확률(0.0~1.0) bool

    template<typename Container>
    auto&  rand_pick(Container& c);                 // 컨테이너에서 랜덤 원소 참조

    template<typename Container>
    void   shuffle(Container& c);                   // 인-플레이스 셔플
};
```

> `rand_discrete`는 `BagDraw`로 대체된다. `RandomEngine`은 순수 균등 난수 기반 연산만 담당한다.

#### 3-2. `IntRange` / `FloatRange` — 범위 값 객체

```cpp
struct IntRange   { int    min; int    max; };
struct FloatRange { double min; double max; };
```

#### 3-3. `BagDraw<T>` — 게임 전용 공 뽑기 랜덤

- Bag Draw 모델: 가중치만큼 공을 가방에 넣고 섞은 뒤 순서대로 뽑는다
- 한 사이클(가방 전체)을 소진하면 다시 채우고 섞는다
- **사이클 내 정확한 비율**을 보장하며 장기 평균도 설계 확률과 일치한다

```
예) 가중치 [3, 1, 1] → 가방 [0,0,0,1,2] 셔플 → 순서대로 pick
    5회 안에 반드시 0번이 3번, 1번·2번이 각 1번씩 등장
```

`random_utils.h`와 분리된 **`game_random.h`** 에 배치한다.
`RandomEngine`을 내부에 보유하고, **시드·크기·현재 인덱스**를 상태로 저장해
서버 재부팅 후에도 정확히 이어서 진행할 수 있다.

##### 상태 값 객체 `BagDrawState`

```cpp
struct BagDrawState {
    uint64_t seed;        // 셔플 재현용 시드
    int      bag_size;    // 가방 전체 공 수 (가중치 합계)
    int      index;       // 다음에 뽑을 위치 (0 ~ bag_size-1)
};
```

- DB/파일에 이 세 값만 저장하면 재부팅 후 동일한 가방 순서를 복원할 수 있다.
- `seed`로 셔플 결과를 재현하므로 공 순서까지 완벽히 복구된다.

##### `BagDraw<T>` 클래스

```cpp
template<typename T>
class BagDraw {
public:
    // 신규 생성 — random_device로 시드 자동 발급
    explicit BagDraw(std::initializer_list<std::pair<T, int>> weighted_items);

    // 재부팅 복원 — 저장된 상태로 동일한 가방 순서 재현
    BagDraw(std::initializer_list<std::pair<T, int>> weighted_items,
            const BagDrawState& saved_state);

    // 다음 공 뽑기 — 가방이 비면 새 시드로 리필 & 셔플
    const T& draw();

    // 현재 상태 스냅샷 — 저장·전송용
    BagDrawState save_state() const;

    // 현재 가방에 남은 공 수
    int remaining() const;
};
```

**동작 방식**
1. 생성 시: 가중치만큼 `T` 값을 `bag_` 벡터에 복제 → 시드 기록 → 셔플
2. `draw()`: `bag_[index_++]` 반환
3. `index_ == bag_size`이면: 새 시드 생성 → 리필 → 셔플 → `index_ = 0`
4. `save_state()`: `{seed_, bag_size_, index_}` 반환 → DB 저장
5. 복원 생성자: 동일 `seed_`로 셔플 재현 후 `index_` 위치에서 재개

#### 3-4. 글로벌 편의 함수 (thread_local RandomEngine 사용)

| 함수                              | 설명 |
|---------------------------------|------|
| `Rand(int min, int max)`        | 글로벌 엔진으로 정수 난수 |
| `Rand(IntRange range)`          | 범위 객체 버전 |
| `Rand(double min, double max)`  | 글로벌 엔진으로 실수 난수 |
| `Rand(FloatRange range)`        | 범위 객체 버전 |
| `Rand_bool(double probability)` | 글로벌 엔진으로 bool |

---

## 파일 레이아웃

```
Lib/include/
└── utils/
    ├── string_utils.h
    ├── container_utils.h
    ├── random_utils.h      ← RandomEngine, IntRange, FloatRange, 글로벌 편의 함수
    └── game_random.h       ← BagDrawState, BagDraw<T> (게임 전용)
```

---

## 구현 방침

- **헤더 온리**: 모든 구현을 헤더에 포함 (`inline` 또는 `constexpr`)
- **C++20**: `std::ranges`, `std::span`, `std::string_view` 적극 활용
- **표준 라이브러리만**: `<string>`, `<vector>`, `<algorithm>`, `<random>`, `<ranges>`, `<span>` 등
- **엔진/플랫폼 비의존**: Boost, spdlog 등 외부 라이브러리 사용 금지
- **네임스페이스**: `portpolio::lib::utils`
- **테스트**: `Lib/tests/` 아래에 각 헤더당 단위 테스트 파일 추가

---

## 진행 단계

- [ ] 1. `Lib/include/utils/string_utils.h` 작성
- [ ] 2. `Lib/include/utils/container_utils.h` 작성
- [ ] 3. `Lib/include/utils/random_utils.h` 작성
- [ ] 4. `Lib/include/utils/game_random.h` 작성
- [ ] 5. `Lib/tests/` 아래 단위 테스트 작성
- [ ] 6. `Lib/include/README.md` 및 `Lib/README.md` 업데이트
- [ ] 7. INDEX.md 완료 처리 및 `completed/`로 이동

---

## 완료 기준

- 네 헤더 파일이 `portpolio::lib::utils` 네임스페이스 아래 구현됨
- 각 함수에 단위 테스트 존재
- 외부 의존성 없이 표준 라이브러리만 사용
- README 업데이트 완료
