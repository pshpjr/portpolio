#!/usr/bin/env python3
"""Validate internal markdown links and active exec-plan sections."""

from __future__ import annotations

import re
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[1]
LINK_PATTERN = re.compile(r"\[.*?\]\((?!https?://)(?!#)([^)]+)\)")
FENCED_CODE_BLOCK_PATTERN = re.compile(r"```.*?```|~~~.*?~~~", re.DOTALL)
EXCLUDED_PARTS = {
    ".git",
    ".idea",
    ".codex",
    ".claude",
    ".cache",
    "build",
    "vcpkg_installed",
    "blds",
}
ACTIVE_PLAN_DIRS = (
    REPO_ROOT / "docs" / "exec-plans" / "active",
    REPO_ROOT / "docs" / "proposal" / "exec-plans" / "active",
    REPO_ROOT / "server" / "docs" / "exec-plans" / "active",
    REPO_ROOT / "Lib" / "docs" / "exec-plans" / "active",
)
PLAN_GOAL_HEADINGS = ("## 목표", "## Goal")
PLAN_COMPLETION_HEADINGS = ("## 완료 기준", "## Completion Criteria")


def iter_markdown_files() -> list[Path]:
    files = []
    for md_file in REPO_ROOT.rglob("*.md"):
        if any(part in EXCLUDED_PARTS for part in md_file.parts):
            continue
        files.append(md_file)
    return files


def strip_fenced_code_blocks(content: str) -> str:
    return FENCED_CODE_BLOCK_PATTERN.sub("", content)


def check_internal_links() -> list[str]:
    errors = []
    for md_file in iter_markdown_files():
        content = strip_fenced_code_blocks(md_file.read_text(encoding="utf-8"))
        for match in LINK_PATTERN.finditer(content):
            link_target = match.group(1).strip()
            if not link_target or link_target.startswith("mailto:"):
                continue
            target_without_anchor = link_target.split("#", 1)[0]
            if not target_without_anchor:
                continue
            target_path = (md_file.parent / target_without_anchor).resolve()
            if not target_path.exists():
                errors.append(
                    f"{md_file.relative_to(REPO_ROOT)}: 깨진 링크 → {link_target}"
                )
    return errors


def check_exec_plans() -> list[str]:
    warnings = []
    for active_dir in ACTIVE_PLAN_DIRS:
        if not active_dir.exists():
            continue
        for plan in active_dir.glob("*.md"):
            if plan.name == "INDEX.md":
                continue
            content = plan.read_text(encoding="utf-8")
            if not any(heading in content for heading in PLAN_COMPLETION_HEADINGS):
                warnings.append(f"{plan.relative_to(REPO_ROOT)}: 완료 기준 섹션 없음")
            if not any(heading in content for heading in PLAN_GOAL_HEADINGS):
                warnings.append(f"{plan.relative_to(REPO_ROOT)}: 목표 섹션 없음")
    return warnings


def main() -> int:
    if hasattr(sys.stdout, "reconfigure"):
        sys.stdout.reconfigure(encoding="utf-8", errors="replace")

    issues: list[str] = []

    link_errors = check_internal_links()
    if link_errors:
        print(f"깨진 내부 링크 {len(link_errors)}건:")
        for error in link_errors:
            print(f"  {error}")
        issues.extend(link_errors)

    plan_warnings = check_exec_plans()
    if plan_warnings:
        print(f"exec-plan 형식 경고 {len(plan_warnings)}건:")
        for warning in plan_warnings:
            print(f"  {warning}")

    if not issues:
        print("OK: documentation checks passed.")
        return 0

    return 1


if __name__ == "__main__":
    sys.exit(main())
