# Harness Improvement Queue

하네스가 작업 중 발견한 재사용 가능한 개선 항목을 남기는 작업 큐입니다.

이 문서는 채팅 로그 보관용이 아닙니다.
다음 에이전트가 바로 집어가서 처리할 수 있는 독립 작업만 남깁니다.

## 언제 남길까

- 작업 중 문서, 프롬프트, 워크플로, 도구 규칙의 빈틈 때문에 우회가 필요했을 때
- 작업은 끝났지만, 같은 문제를 다음 세션에서도 반복할 가능성이 높을 때
- 현재 메인 작업과 분리 가능한 후속 정리가 남았을 때

## 무엇을 남기지 말까

- 지금 세션 안에서 바로 고칠 수 있는 사소한 메모
- 사용자의 의사결정이 먼저 필요한 미확정 사항
- 장문의 사건 서술 또는 대화 복사본

## 항목 형식

각 항목은 아래 필드를 포함한다.

```markdown
## YYYY-MM-DD - short-title
- status: open | claimed | done | dropped
- suggested-agent: harness-improver | docs-reviewer | reviewer | worker | other
- area: workflow | docs | agent-prompt | skill | tool | build | test | other
- recommended-artifact: script | skill | prompt | doc | other
- context-savings: low | medium | high | unknown
- token-meter: `<estimated tokens / changed lines>` | pending
- task: 다음 에이전트가 수행할 작업 한 줄
- message: 왜 이 작업이 필요한지, 어디서 드러났는지, 다음 에이전트가 알아야 할 핵심 맥락
- source: 발견한 작업 또는 파일 경로
```

핵심 규칙:

- `task`는 명령형으로 짧게 쓴다.
- `message`는 재현 맥락과 의도를 남긴다.
- 반복 설명이나 반복 생성이 원인이라면 `recommended-artifact`에 `script` 또는 `skill`을 명시한다.
- 컨텍스트 / 토큰 절감 기대치가 보이면 `context-savings`에 강도를 남긴다.
- substantial item이면 `token-meter`에 계측 결과를 남기고, 아직 안 쟀으면 `pending`으로 둔다.
- 하나의 항목에는 하나의 독립 작업만 남긴다.
- 처리자가 정해지면 `status: claimed`와 함께 본문에 owner 메모를 덧붙여도 된다.

## 운영 방식

1. 메인 작업 에이전트는 작업 중 또는 종료 직전에 큐 필요 여부를 판단한다.
2. 독립 후속 작업이면 이 파일의 `Open Tasks`에 새 항목을 추가한다.
3. `harness-improver` 또는 더 적합한 리뷰/구현 에이전트가 항목을 claim 한다.
4. 처리 후 `python tools/hi_done.py <short-title>` 로 완료 처리한다 (큐에서 제거 + done 파일 이동).

## Open Tasks

## 2026-04-16 - migrate-server-includes-combat-to-game-data
- status: open
- suggested-agent: worker
- area: build
- recommended-artifact: other
- context-savings: low
- token-meter: pending
- task: 서버 소스에서 `combat_data_tables.h`, `combat_data_enums.h`, `combat_data_common.h`, `combat/*.h` 인클루드를 `game_data/game_data_tables.h`, `game_data/enums.h`, `game_data/table_common.h`, `game_data/*.h`로 교체하고 `psh::generated::combat` 네임스페이스 참조를 `psh::generated`로 변경한다.
- message: `game_data_gen.py` Jinja2 리팩터링으로 서버 생성 경로가 `generated/game_data/`로 이동, 네임스페이스가 `psh::generated`로 변경됨. 기존 `generated/combat_data_*.h`와 `generated/combat/` 파일은 새 생성 결과로 대체되었으나, 비생성 서버 소스의 인클루드가 아직 구 경로를 가리킴. `CombatDataBundle` → `GameDataBundle` 타입명도 변경 필요.
- source: server/tools/codegen/game_data_gen.py 리팩터링

## 2026-04-16 - migrate-unreal-includes-combat-to-game-data
- status: open
- suggested-agent: worker
- area: build
- recommended-artifact: other
- context-savings: low
- token-meter: pending
- task: 언리얼 클라이언트 소스에서 `Generated/combat_data_tables.h`, `Generated/combat_data_enums.h`, `Generated/combat/*.h` 인클루드를 `Generated/game_data/game_data_tables.h`, `Generated/game_data/enums.h`, `Generated/game_data/*.h`로 교체한다. `FCombatDataBundle` → `FGameDataBundle`, `kCombatDataTables` → `kGameDataTables` 타입/변수명도 변경.
- message: 언리얼도 동일하게 `game_data/` 하위 플랫 구조로 생성 경로 변경됨. 기존 `combat/` 서브디렉터리와 `combat_data_*` 접두사 파일은 더 이상 생성되지 않음.
- source: server/tools/codegen/game_data_gen.py 리팩터링

## 2026-04-16 - cleanup-old-combat-generated-files
- status: open
- suggested-agent: worker
- area: build
- recommended-artifact: other
- context-savings: low
- token-meter: pending
- task: 인클루드 마이그레이션 완료 후 `server/src/generated/combat_data_*.h`, `server/src/generated/combat/`, `client/.../Generated/combat_data_*.h`, `client/.../Generated/combat/` 구 파일 삭제. `combat_data_gen.py`도 삭제 또는 deprecated 마킹.
- message: 새 `game_data_gen.py`가 대체하므로 구 파일은 빌드 혼란 방지를 위해 제거해야 함. 인클루드 마이그레이션이 먼저 완료되어야 안전하게 삭제 가능.
- source: server/src/generated/, client/Source/client/Public/Generated/

## 2026-04-08 - review-delegate-opencode-text-extraction-robustness
- status: open
- suggested-agent: harness-improver
- area: tool
- recommended-artifact: script
- context-savings: high
- token-meter: pending
- task: `tools/review_delegate.sh`의 opencode JSON 후처리를 실제 다양한 출력으로 검증하고 edge case 보강
- message: opencode `--format json` 출력에서 마지막 `type:text` 이벤트의 `.part.text`를 추출하는 로직이 단일 테스트(짧은 프롬프트)만 거쳤다. 긴 리뷰, 멀티 턴, 에러 시 빈 출력 등 edge case 검증 필요. jq 없는 환경의 python fallback도 테스트 필요.
- source: tools/review_delegate.sh

## 2026-04-05 - decide-whether-response-artifacts-should-also-capture-explic
- status: dropped
- decision: summary-only — review-only 실행의 command output은 agent-notes에 저장하지 않는다.
- source: recursive harness improvement rollout

## Claimed

없음.

## Done

완료 항목은 [harness-improvement-done.md](./harness-improvement-done.md)에서 관리합니다.
