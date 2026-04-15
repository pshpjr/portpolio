#!/usr/bin/env python3
"""Create a new exec-plan file from the standard template and register it in INDEX.md.

Usage:
    python tools/new_exec_plan.py --name harness-token-optimization --area docs
    python tools/new_exec_plan.py --name combat-skill-refactor --area server --goal "전투 스킬 레이어 분리"
"""

from __future__ import annotations

import argparse
import re
import sys
from datetime import date
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[1]

AREA_DIRS: dict[str, Path] = {
    "docs": REPO_ROOT / "docs" / "exec-plans" / "active",
    "proposal": REPO_ROOT / "docs" / "proposal" / "exec-plans" / "active",
    "server": REPO_ROOT / "server" / "docs" / "exec-plans" / "active",
    "lib": REPO_ROOT / "Lib" / "docs" / "exec-plans" / "active",
}

TEMPLATE = """\
# Exec Plan: {title}

## 목표

{goal}

## 범위

- 수정/생성할 파일 또는 디렉터리 목록
- 영향받는 레이어 또는 도메인

## 완료 기준

- [ ] 구체적인 완료 기준 1
- [ ] 구체적인 완료 기준 2
- [ ] doc_check.py 통과

## 진행 상황

- [ ] 항목 1
- [ ] 항목 2

## 설계 결정

작업 중 내린 결정과 이유를 여기에 기록한다.
"""


def slugify(text: str) -> str:
    text = text.strip().lower()
    text = re.sub(r"[^a-z0-9]+", "-", text)
    return text.strip("-") or "task"


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Create a new exec-plan file and register it in the active INDEX.md."
    )
    parser.add_argument(
        "--name",
        required=True,
        help="Short slug for the plan name (e.g. 'harness-token-opt'). Used in file name.",
    )
    parser.add_argument(
        "--area",
        choices=list(AREA_DIRS),
        default="docs",
        help="Which exec-plan directory to use. Default: docs.",
    )
    parser.add_argument(
        "--goal",
        default="한 문장으로 이 작업이 달성해야 할 것.",
        help="One-line goal to fill in the 목표 section.",
    )
    parser.add_argument(
        "--description",
        default="",
        help="Short description for the INDEX.md entry. Defaults to --goal.",
    )
    args = parser.parse_args()

    active_dir = AREA_DIRS[args.area]
    active_dir.mkdir(parents=True, exist_ok=True)

    today = date.today().strftime("%Y%m%d")
    name_slug = slugify(args.name)
    filename = f"{today}-{name_slug}.md"
    plan_path = active_dir / filename

    if plan_path.exists():
        print(f"ERROR: plan already exists: {plan_path.relative_to(REPO_ROOT)}", file=sys.stderr)
        return 1

    title = args.name.replace("-", " ").title()
    content = TEMPLATE.format(title=title, goal=args.goal)
    plan_path.write_text(content, encoding="utf-8")
    print(f"Created: {plan_path.relative_to(REPO_ROOT)}")

    # Register in INDEX.md
    index_path = active_dir / "INDEX.md"
    if index_path.exists():
        description = args.description or args.goal
        index_text = index_path.read_text(encoding="utf-8")
        entry = f"\n- [{filename}](./{filename}) - {description}"

        marker = "## 현재 활성 계획"
        if marker in index_text:
            updated = index_text.replace(marker, marker + entry, 1)
            index_path.write_text(updated, encoding="utf-8")
            print(f"Registered in: {index_path.relative_to(REPO_ROOT)}")
        else:
            print(f"WARNING: INDEX.md missing '## 현재 활성 계획' — skipped registration.")
    else:
        print(f"NOTE: INDEX.md not found at {index_path.relative_to(REPO_ROOT)} — skipped registration.")

    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except Exception as exc:  # pragma: no cover
        print(f"ERROR: {exc}", file=sys.stderr)
        raise SystemExit(1)
