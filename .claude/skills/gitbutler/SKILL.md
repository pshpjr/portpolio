---
name: gitbutler
description: "Git Butler(but CLI) 환경에서의 git 작업 가이드. 현재 브랜치가 gitbutler/workspace이거나 gitbutler/* 패턴일 때 자동으로 적용한다. 커밋, push, PR, 브랜치 관리 등 모든 VCS 작업에 git 명령 대신 but 명령을 사용한다. git commit 대신 but commit, git push 대신 but push를 쓴다."
---

# Git Butler CLI (but) 작업 가이드

Git Butler 환경에서는 `git` 대신 `but` CLI를 사용한다.
경로: `"/c/Program Files/GitButler/but.exe"` (bash에서 따옴표 필수)

## 명령어 매핑

| 작업 | git (사용 금지) | but (사용) |
|------|-----------------|------------|
| 상태 확인 | `git status` | `but status` |
| 커밋 | `git commit -m "msg"` | `but commit -m "msg" [branch]` |
| 푸시 | `git push` | `but push [branch]` |
| 브랜치 목록 | `git branch` | `but branch` |
| 브랜치 생성 | `git checkout -b name` | `but branch new name` |
| 스테이징 | `git add file` | `but stage <file-id> <branch>` |
| 변경 취소 | `git checkout -- file` | `but discard <id>` |
| PR 생성 | `gh pr create` | `but pr new` |
| upstream 동기화 | `git pull` | `but pull` |
| 커밋 메시지 수정 | `git commit --amend` | `but reword <commit>` |
| 변경 흡수 | - | `but absorb` |
| 되돌리기 | `git revert` | `but undo` |

## 커밋 워크플로

```bash
# 1. 상태 확인 — 파일 ID와 브랜치 확인
but status

# 2. 브랜치가 하나면 바로 커밋
but commit -m "feat: add feature"

# 3. 브랜치가 여러 개면 대상 브랜치 지정
but commit -m "feat: add feature" lo

# 4. AI 커밋 메시지 생성
but commit --ai

# 5. 특정 파일만 커밋 (-p 플래그로 파일/헝크 ID 지정)
but commit -m "fix: bug" -p a1,a2
```

## 스테이징

`but status`에서 보이는 파일/헝크 ID를 사용한다.

```bash
# 특정 파일을 특정 브랜치에 스테이징
but stage a1 lo

# 인터랙티브 TUI (터미널에서 직접 사용 시)
but stage
```

## push / PR

```bash
# 브랜치 push
but push lo

# 모든 unpushed 브랜치 push (비대화 모드)
but push

# PR 생성
but pr new

# 드래프트 PR
but pr new --draft
```

## 읽기 전용 git 명령은 허용

다음은 상태를 변경하지 않으므로 `git`을 직접 써도 된다:
- `git log`, `git diff`, `git blame`, `git show`
- `git fetch origin`

## 금지 명령

가상 브랜치 상태를 깨뜨리는 git 쓰기 명령:
- `git commit`, `git merge`, `git rebase`, `git reset`
- `git checkout <branch>`, `git switch`
- `git push`, `git pull`
- `git branch -D gitbutler/*`