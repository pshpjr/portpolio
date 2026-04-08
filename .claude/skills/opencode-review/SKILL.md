---
name: opencode-review
description: Delegate a review or inspection task to the local OpenCode CLI through headless `opencode run` for a second opinion. Use after Claude's own review, or when an independent perspective is needed on code, documentation, or proposal content.
---

# OpenCode Review Delegation Skill

Request an additional review from the local OpenCode CLI using the project's headless agent setup.
Use this skill to obtain a second perspective and reduce blind spots.

## When to Use

- When an independent second opinion is needed before a large PR or architecture change
- When additional consistency validation is needed after a documentation restructure
- When examining planning decisions from multiple angles for logical flaws
- When Claude has finished a review but confidence is low

## Basic Workflow

1. **Claude goes first** — complete a primary analysis using its own review or a relevant agent (reviewer, docs-reviewer, etc.).
2. **Compile the list of files to review and the relevant context.**
3. **Delegate to the `opencode-reviewer` sub-agent.**
   - Specify the review type: code / documentation / planning
   - Provide the list of target file paths
   - Provide the key evaluation criteria (layer rules, role-separation principles, etc.)
   - The sub-agent will call `opencode run --agent portpolio-review`
4. **Combine the OpenCode result with Claude's result** to reach a final conclusion.

## Example Delegation Prompt

```
Content to pass to opencode-reviewer:

Review type: Code review
Target file: server/src/03_core/combat/CombatSystem.cpp
Evaluation criteria:
  - Layer rule: 03_core must not depend on 04_service or 05_network
  - Single responsibility: check whether any class carries too many roles
  - Potential bugs: null checks, bounds checks, race conditions
```

## Result Integration Principles

- OpenCode and Claude agree → high-confidence conclusion
- Only OpenCode finds an issue → that finding must be re-examined
- Only Claude finds an issue → retain it (OpenCode not finding it is not a refutation)
- They conflict → present both perspectives to the user and ask for a judgment call

## After Review: Clear Fix Path

리뷰 결과로 수정 방향이 명확해진 경우, OpenCode direct-apply 모드로 즉시 위임할 수 있다.

| 상황 | 경로 |
|------|------|
| 수정 범위가 모호하거나 설계 판단이 필요한 경우 | Claude가 직접 수정 |
| 수정할 파일과 내용이 명확하고 OpenCode가 단독 쓰기 소유권을 갖는 경우 | OpenCode direct-apply |

### OpenCode Direct-Apply 위임 방법

```bash
opencode run --agent portpolio-implement --format json "[수정 작업 프롬프트]"
```

- 프롬프트에는 수정 대상 파일 경로, 수정 내용, 검증 방법을 명시한다.
- 수정 완료 후 Claude가 결과를 확인하고 exec-plan을 갱신한다.
- write 소유권이 분리되지 않는 경우에는 proposal 모드(`portpolio-propose`)를 사용한다.

## Notes

- The repository does not pin an OpenCode model. `opencode run` uses the local OpenCode provider/model configuration.
- If `opencode` is not installed or the headless run fails, skip this skill and report the reason.
- OpenCode results are a second opinion. They do not replace Claude's judgment.
- Review results do not need to be recorded in the exec-plan (volatile review opinions should not live in the plan).
