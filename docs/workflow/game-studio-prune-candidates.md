# Game Studio 병합 후 제거/이관 후보 목록

> 작성일: 2026-04-17
> 목적: 대량 병합 이후 **기존 자산 중 중복/무의미 후보**를 먼저 분류해 안전한 정리 순서를 만든다.

## 1) 에이전트 제거/통합 후보

### A. 우선 제거 후보

- 2026-04-17 기준 legacy critic agents 및 `.codex/agents/` wrapper 제거 완료

### B. 통합 후 축소 후보 (즉시 삭제보다 단계적 이관 권장)

| 기존 agent | 제안 | 이유 | 선행 점검 |
|---|---|---|---|
| `worker` | 범용 fallback 1개로 축소 유지 | 자동화/일반 작업 fallback 필요는 있으나 도메인 실행은 Studio 역할로 이전 가능 | `.claude/settings.json`의 허용 목록 점검 |
| `reviewer` | `qa-lead`/`technical-director`/`docs-reviewer`로 분리 | 단일 reviewer보다 도메인별 리뷰 체계가 명확 | `.claude/settings.json`, `docs/workflow/harness-improvement-queue.md` |
| `scaffolder` | `lead-programmer` + `tools-programmer` 조합으로 이관 | 스캐폴딩 역할이 기술 리드/툴링 역할과 중복 | `.claude/settings.json`, `.codex/agents/scaffolder.md` |

## 2) 스킬 제거/통합 후보

### A. 우선 통합 후보

| 기존 skill | 대체 후보 | 판단 근거 | 선행 수정 파일 |
|---|---|---|---|
| `performance-analysis` | `perf-profile` | 성능 분석 목적이 실질적으로 동일 | `.codex/skills/performance-analysis/SKILL.md` |
| `tradeoff-resolution` | `architecture-decision`, `scope-check` | 트레이드오프 결정 템플릿이 Studio 의사결정 스킬과 중복 | `.codex/skills/tradeoff-resolution/SKILL.md` |

### B. 유지 권장 (Studio와 역할이 다름)

- `doc-governance`, `harness-improvement`, `codex-delegate`, `opencode-review`
- 이유: 저장소 운영 규칙/도구 체인과 직접 결합되어 Studio 도메인 스킬로 대체 불가

## 3) agent.md/문서에서 옮기거나 지울 참조 포인트

| 파일 | 현재 상태 | 조치 제안 |
|---|---|---|
| `.claude/settings.json` | `worker/reviewer/scaffolder` 중심 허용 목록 | Studio 핵심 역할(`producer`, `technical-director`, `qa-lead`, `lead-programmer`)로 갱신 검토 |
| `.codex/agents/*.md` (legacy critic들) | 삭제 후보 agent를 canonical로 참조 | 대상 agent 삭제 시 wrapper 동시 정리 |

## 4) 실행 순서 (권장)

1. **참조 정리 먼저**: skill/doc/settings에서 legacy agent 참조 제거
2. **wrapper 정리**: (완료) `.codex/agents/`의 legacy wrapper 삭제
3. **agent/skill 실삭제**: 실제 파일 삭제
4. **검증**: `python tools/doc_check.py` + 필요한 smoke check

## 5) 주의사항

- `design-author`, `data-table-reviewer`, `docs-*` 계열은 현재 문서 체인과 직접 연결되어 있어 즉시 삭제 금지
- active/completed exec-plan에서 과거 에이전트 이름은 기록 보존이 목적이므로, 히스토리 문서는 원칙적으로 수정하지 않음
