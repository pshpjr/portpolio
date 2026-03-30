# Game Design Agent Roundtable

## 목표
성격과 관점이 다른 게임 기획자 agent 세트를 추가하고, 이를 이용해 `Dungeon of Spirits` 기획서의 수정 후보를 빠르게 토론할 수 있게 만든다.

## 범위
- `.codex/agents/gameplay-loop-critic.md`
- `.codex/agents/co-op-flow-advocate.md`
- `.codex/agents/economy-and-progression-skeptic.md`
- `.codex/agents/portfolio-scope-guardian.md`
- `.codex/agents/fantasy-theme-curator.md`
- `.claude/agents/gameplay-loop-critic.md`
- `.claude/agents/co-op-flow-advocate.md`
- `.claude/agents/economy-and-progression-skeptic.md`
- `.claude/agents/portfolio-scope-guardian.md`
- `.claude/agents/fantasy-theme-curator.md`
- `docs/proposal/exec-plans/active/INDEX.md`
- `docs/proposal/exec-plans/active/20260330-game-design-agent-roundtable.md`

## 완료 기준
- [x] 서로 다른 관점과 성격을 가진 게임 기획 agent 초안을 정의한다.
- [x] Codex/Claude 공용 agent 프롬프트를 같은 역할 세트로 맞춘다.
- [x] agent들을 활용한 첫 토론 라운드를 실행한다.
- [x] 토론 결과에서 우선 수정 후보를 설계 메모에 남긴다.
- [x] `python ../../tools/doc_check.py` 검증 결과를 기록한다.
- [ ] 변경분만 선별 커밋한다.

## 진행 상황
- [x] `docs/proposal/main.md`와 active exec-plan 인덱스를 확인했다.
- [x] 기존 `.codex/agents/`와 `.claude/agents/` 구조를 확인했다.
- [x] 게임 기획 토론용 agent 역할 세트와 파일 경로를 정했다.
- [x] proposal 토론용 agent 프롬프트 초안을 추가했다.
- [x] 각 agent로 첫 토론 라운드를 실행했다.
- [x] 토론 결과를 설계 메모에 정리했다.
- [x] `python ../../tools/doc_check.py`를 실행했다.
- [ ] 변경분만 선별 커밋한다.

## 설계 메모
- 이 프로젝트는 "재미있는 협동 액션 RPG"와 "취업용 고성능 서버 포트폴리오"라는 두 목표를 동시에 만족해야 하므로, fun/협동/경제/테마/스코프를 분리해서 비판하는 agent 구성이 효율적이다.
- agent 프롬프트는 모두 `docs/proposal/AGENTS.md`와 active plan을 먼저 읽게 해 기획 문서 라우팅 규칙을 어기지 않도록 한다.
- Codex와 Claude 양쪽에 같은 프롬프트를 두면 도구가 달라도 같은 관점 세트를 재사용할 수 있다.
- 첫 토론 라운드에서 공통으로 드러난 강점은 `패링/무력화/무기 정체성` 전투 코어와 `단일 스레드 인던 + 성능 리포트`라는 포트폴리오 메시지다.
- 반복 지목된 1순위 수정 후보는 `docs/proposal/main.md`에 `v1 showcase slice`와 명시적 defer 목록을 추가해 첫 완성본의 범위를 더 날카롭게 고정하는 것이다.
- 전투 문서 쪽에서는 `docs/proposal/ingame/combat.md`에 실전 우선순위 사다리를 넣고, `docs/proposal/common/skill.md`와 `docs/proposal/ingame/boss_pattern.md`에서 입력 종류와 패턴 예산을 줄여 과밀한 조작/암기 부담을 낮추는 제안이 나왔다.
- 협동 흐름 쪽에서는 `docs/proposal/outgame/party.md`, `docs/proposal/outgame/dungeon_entry.md`, `docs/proposal/outgame/chat.md`, `docs/proposal/ingame/dungeon_progress.md`에 `READY` blocker 가시화, 짧은 quick-callout, 실패 후 regroup 구간을 넣어 마찰과 blame loop를 줄이라는 의견이 나왔다.
- 경제 쪽에서는 `docs/proposal/common/item.md`, `docs/proposal/outgame/economy.md`, `docs/proposal/ingame/dungeon_progress.md`에 smart drop을 완전 고정이 아닌 soft bias로 완화하고, sink ladder와 반복 파밍 보상 분리를 더 선명하게 적으라는 의견이 나왔다.
- 테마 쪽에서는 `docs/proposal/main.md`, `docs/proposal/ingame/combat.md`, `docs/proposal/common/item.md`, `docs/proposal/outgame/dungeon_entry.md`, `docs/proposal/ingame/boss_pattern.md`에 시스템 용어를 판타지 언어로 한 단계 더 번역해 프로젝트의 기억점을 강화하라는 의견이 나왔다.
- `python ../../tools/doc_check.py` 실행 결과 문서 검증은 통과했다.
