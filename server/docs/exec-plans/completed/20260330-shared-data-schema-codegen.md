# Exec Plan: Shared Data Schema Codegen

## Goal

인게임 데이터 원본을 단일 스키마 포맷으로 정리하고, 변경 시 서버가 참조하는 DTO와 언리얼이 로딩하는 타입/메타가 함께 재생성되는 파이프라인을 도입한다.

---

## Scope

### Source Schema 정의

- `docs/proposal/ingame/data/` 아래의 원본 포맷을 단일 소스 오브 트루스로 사용한다.
- 1차 도입 포맷은 `schema.json + data.json` 조합으로 고정한다.
- 테이블 이름, 필드 이름, 필드 순서, enum, key, nullable, Unreal 노출 여부를 스키마에서 명시한다.

### 생성 산출물

- `shared/generated/server/` 아래에 서버용 DTO 헤더와 로더 메타를 생성한다.
- `shared/generated/unreal/` 아래에 언리얼용 `USTRUCT` 또는 동등한 로딩 타입 선언을 생성한다.
- 필요하면 언리얼에서 바로 소비할 수 있는 JSON 또는 DataTable용 보조 산출물을 함께 만든다.
- 생성 파일에는 `Auto-generated. Do not edit.` 배너를 넣는다.

### 빌드 연동

- `server/CMakeLists.txt` 의 기존 packet codegen 훅과 같은 패턴으로 데이터 codegen을 연결한다.
- 입력 schema 또는 data가 바뀌면 다음 configure/build에서 generator가 재실행되게 한다.
- 서버와 언리얼이 공용 generated 산출물을 각자 참조할 수 있게 경로를 정리한다.

### 범위 외

- 런타임 DB 저장/마이그레이션
- 밸런스 로직 자체를 Python generator로 이동하는 작업
- 에디터 전용 제작 툴의 대규모 UI 구현

---

## Completion Criteria

- 스키마 변경이 generator 재실행으로 서버/언리얼 산출물에 함께 반영된다.
- 서버 코드가 생성된 DTO 헤더를 include할 수 있다.
- 언리얼이 생성된 타입으로 데이터를 로딩할 수 있다.
- 필수 타입 정보가 누락되면 generator가 명확한 오류로 실패한다.
- 관련 generator 단독 실행과 서버 build hook 경로가 검증된다.

---

## Progress

- [completed] 현재 CSV 위치와 생성 스크립트, 기존 packet codegen 빌드 훅을 조사했다.
- [completed] 작업 초안을 active exec-plan으로 등록했다.
- [completed] 언리얼 타입 생성 필요 조건을 반영해 plan 범위를 확장했다.
- [completed] source schema 포맷을 `schema.json + data.json` 으로 고정했다.
- [completed] generator 구현
- [completed] 서버/언리얼 산출물 형태 확정
- [completed] build hook 연결
- [completed] focused verification 수행

---

## Design Decision Log

- 언리얼까지 포함하면 단순 CSV 헤더 추론만으로는 `USTRUCT` 필드 타입, enum 매핑, `FName/FText/FString`, `BlueprintType` 노출 기준을 안정적으로 결정하기 어렵다.
- 1차 구현은 외부 Python 패키지 없이 동작하는 generator를 우선하므로, source-of-truth를 `schema.json + data.json` 으로 통일한다.
- YAML schema는 사람 친화적이지만 표준 라이브러리만으로 안정적 파싱을 보장하기 어려워 이번 범위에서는 제외한다.
- generator 위치는 저장소 공용 입력을 읽되 서버 빌드에 묶이므로 `server/tools/codegen/` 이 가장 자연스럽다.
- 생성 출력은 기존 패턴에 맞춰 `shared/generated/server/`, `shared/generated/unreal/` 아래에 두고, 각 런타임은 generated include만 소비한다.
- 언리얼 산출물은 현재 `shared/generated/unreal/` 외부 include 경로에서 바로 소비해야 하므로, 1차 구현에서는 UHT가 필요한 `USTRUCT/UENUM` 대신 Unreal 타입(`FString`, `FName`, `FText`)을 쓰는 plain C++ struct/enum으로 생성한다.
- focused verification 결과:
  - `python server/tools/codegen/combat_data_gen.py` 통과
  - `python -m py_compile server/tools/codegen/combat_data_gen.py` 통과
  - `python tools/doc_check.py` 통과
  - `cmake -S server -B server/build/codex-combat-data-check -G Ninja` 는 로컬 C++ compiler 미설정으로 실패
