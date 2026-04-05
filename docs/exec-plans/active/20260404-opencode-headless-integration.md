# Exec Plan: OpenCode Headless Integration

## 목표

저장소의 OpenCode 연동을 HTTP 웹 호출에서 프로젝트 레벨 헤드리스 호출로 전환해, 리뷰와 구현 위임 모두를 일관된 로컬 실행 경로로 정리한다.

## 범위

- 루트 `opencode.json` 프로젝트 설정 추가
- 루트 `.opencode/agents/` 프로젝트 전용 OpenCode 에이전트 추가
- `.claude/agents/opencode-reviewer.md` 헤드리스 호출 방식으로 갱신
- `.claude/skills/opencode-review/SKILL.md` 헤드리스 사용 기준으로 갱신
- `.claude/skills/codex-delegate/SKILL.md` OpenCode 구현 위임 경로 갱신
- 필요 시 `.codex/skills/opencode-review/` 메타데이터와 `docs/workflow/agent-workflow.md` 설명 동기화

## 완료 기준

- [x] 루트에 체크인 가능한 `opencode.json`이 추가되어 프로젝트 레벨 설정이 적용된다
- [x] `.opencode/agents/`에 최소 2개 이상의 프로젝트 전용 에이전트가 추가된다
- [x] 저장소 내 OpenCode 위임 문서가 더 이상 legacy HTTP API를 기본 경로로 안내하지 않는다
- [x] 리뷰 전용과 수정 가능 경로의 역할 차이가 문서에 명확히 기록된다
- [x] 문서 작업 기준으로 필요한 검증 결과가 기록된다

## 진행 상황

- [x] 공식 문서로 헤드리스 호출(`opencode run`)과 프로젝트 설정(`opencode.json`, `.opencode/agents/`) 지원 여부 확인
- [x] 저장소 내 기존 OpenCode 사용 지점 식별
- [x] 루트 exec-plan 생성 및 active index 연결
- [x] 프로젝트 레벨 OpenCode 설정 및 에이전트 추가
- [x] 저장소 스킬/에이전트 문서를 헤드리스 기준으로 갱신
- [x] 관련 문서 검증 및 결과 기록

## 검증 기록

- `python tools/doc_check.py` → `OK: documentation checks passed.`
- 참고: 기존 active plan 형식 경고 3건이 함께 출력되었으나, 이번 작업 파일의 차단 이슈는 아니었다.
- 참고: 현재 Codex 실행 환경에서는 `opencode` CLI가 설치되어 있지 않아 실제 headless 실행까지는 검증하지 못했다. 이번 작업은 프로젝트 레벨 설정과 문서 경로 정리에 집중했다.

## 설계 결정

- 기본 호출 경로는 `opencode run`으로 둔다. `serve`는 세션 재사용이나 attach가 필요할 때만, `acp`는 외부 도구가 프로토콜 통합을 요구할 때만 보조 경로로 취급한다.
- 프로젝트 전용 설정은 루트 `opencode.json`과 `.opencode/agents/`에 둔다. 이렇게 해야 OpenCode 공식 로딩 규칙과 저장소 공용 설정 관리가 일치한다.
- 리뷰와 구현은 별도 OpenCode 에이전트로 분리한다. 리뷰 전용 에이전트는 수정 도구를 막고, 구현 에이전트만 읽기/수정/쉘 도구를 허용한다.
