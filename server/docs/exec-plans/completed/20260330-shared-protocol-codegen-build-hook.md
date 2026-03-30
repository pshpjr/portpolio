# Exec Plan: Shared Protocol Codegen Build Hook

## Goal

`shared/protocol/` 아래의 Python codegen을 서버 빌드 과정에서 자동 실행하도록 연결한다.

---

## Scope

- `server/CMakeLists.txt` 에 Python codegen custom command/target 추가
- `server/tools/codegen/packet_gen.py` 를 빌드 훅에 맞는 최소 CLI로 정리
- `shared/protocol/` 를 입력 경로로, 서버가 참조할 생성 출력 경로를 연결

### Out of Scope

- 언리얼 빌드 파이프라인 연동
- 스키마 포맷 자체 재설계
- 생성 코드의 대규모 템플릿 확장

---

## Completion Criteria

- 서버 빌드 시 packet codegen이 자동 실행된다.
- codegen 입력은 `shared/protocol/` 를 기준으로 한다.
- 서버 타깃이 생성 산출 경로를 include 하도록 연결된다.
- 관련 검증 명령으로 configure/build가 통과한다.

---

## Progress

- [completed] exec-plan 사용자 검토 및 승인
- [completed] CMake build hook 추가
- [completed] Python generator CLI 정리
- [completed] focused verification 수행

---

## Design Notes

- 저장소 구조를 과하게 확장하지 않고 기존 `shared/protocol/` 을 그대로 사용한다.
- generator는 현재 위치인 `server/tools/codegen/` 에 둔 채, 출력만 shared 하위 생성 디렉터리로 보낸다.
- 서버 빌드에서는 stamp 기반 custom command를 사용해 입력 변경 시 재실행되도록 한다.
- `shared/protocol/build-proto.py` 는 외부 Python 패키지 없이 동작하도록 표준 라이브러리만 사용한다.
- 현재 `shared/protocol/` 에 `.proto` 파일이 없을 때는 출력 디렉터리만 준비하고 성공 종료한다.

---

## Verification Notes

- `python server/tools/codegen/packet_gen.py` : 통과
- `cmake -S server -B server/build/codex-codegen-check -G Ninja` : 로컬 C++ compiler 미설정으로 실패
