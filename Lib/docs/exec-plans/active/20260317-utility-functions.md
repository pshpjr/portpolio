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
| `split(str, delim)` | 구분자로 분리 → `vector<string_view>` |
| `starts_with(str, prefix)` | 접두사 확인 (C++17 호환) |
| `ends_with(str, suffix)` | 접미사 확인 (C++17 호환) |
| `join(range, delim)` | 문자열 범위를 구분자로 결합 |

### 2. 컨테이너 헬퍼 (`container_utils.h`)

| 함수 | 설명 |
|------|------|
| `contains(container, value)` | 값 존재 여부 확인 |
| `find_if_or(container, pred, default)` | 조건 검색, 없으면 기본값 반환 |
| `erase_if(container, pred)` | 조건 만족 요소 일괄 삭제 |
| `transform_to_vec(range, func)` | range → `vector` 변환 |
| `map_keys(map)` / `map_values(map)` | map에서 key/value 벡터 추출 |

### 3. 랜덤 유틸리티 (`random_utils.h`)

| 함수 | 설명 |
|------|------|
| `rand_int(min, max)` | `[min, max]` 균등 정수 난수 |
| `rand_float(min, max)` | `[min, max)` 균등 실수 난수 |
| `rand_bool(probability)` | 확률(0.0~1.0)로 bool 반환 |
| `rand_pick(container)` | 컨테이너에서 랜덤 원소 참조 반환 |
| `shuffle(container)` | 컨테이너 인-플레이스 셔플 |

---

## 파일 레이아웃

```
Lib/include/
└── utils/
    ├── string_utils.h
    ├── container_utils.h
    └── random_utils.h
```

---

## 구현 방침

- **헤더 온리**: 모든 구현을 헤더에 포함 (`inline` 또는 `constexpr`)
- **표준 라이브러리만**: `<string>`, `<vector>`, `<algorithm>`, `<random>`, `<ranges>` (C++20) 등
- **엔진/플랫폼 비의존**: Boost, spdlog 등 외부 라이브러리 사용 금지
- **네임스페이스**: `portpolio::lib::utils`
- **테스트**: `Lib/tests/` 아래에 각 헤더당 단위 테스트 파일 추가

---

## 진행 단계

- [ ] 1. `Lib/include/utils/string_utils.h` 작성
- [ ] 2. `Lib/include/utils/container_utils.h` 작성
- [ ] 3. `Lib/include/utils/random_utils.h` 작성
- [ ] 4. `Lib/tests/` 아래 단위 테스트 작성
- [ ] 5. `Lib/include/README.md` 및 `Lib/README.md` 업데이트
- [ ] 6. INDEX.md 완료 처리 및 `completed/`로 이동

---

## 완료 조건

- 세 헤더 파일이 `portpolio::lib::utils` 네임스페이스 아래 구현됨
- 각 함수에 단위 테스트 존재
- 외부 의존성 없이 표준 라이브러리만 사용
- README 업데이트 완료
