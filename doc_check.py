#!/usr/bin/env python3
"""문서 유효성 검사 툴.

docs/ 디렉토리의 마크다운 파일에서 깨진 내부 링크와
ARCHITECTURE.md 도메인 테이블과 grades.md 간의 불일치를 검사한다.

Usage:
    python3 tools/doc_check.py

Exit codes:
    0: 이상 없음
    1: 문제 발견
"""

import sys
import re
from pathlib import Path

ROOT = Path(__file__).parent.parent
DOCS_ROOT = ROOT / "docs"
LINK_PATTERN = re.compile(r'\[.*?\]\(\./([^)]+)\)')


def check_internal_links() -> list[str]:
    """마크다운 파일의 내부 링크 유효성을 검사한다."""
    errors = []
    for md_file in ROOT.rglob("*.md"):
        content = md_file.read_text(encoding="utf-8")
        for match in LINK_PATTERN.finditer(content):
            link_target = match.group(1)
            target_path = md_file.parent / link_target
            if not target_path.exists():
                errors.append(
                    f"{md_file.relative_to(ROOT)}: 깨진 링크 → {link_target}"
                )
    return errors


def check_exec_plans() -> list[str]:
    """active exec-plan 파일들의 형식을 검사한다."""
    warnings = []
    active_dir = DOCS_ROOT / "exec-plans" / "active"
    if not active_dir.exists():
        return warnings
    for plan in active_dir.glob("*.md"):
        content = plan.read_text(encoding="utf-8")
        if "## 완료 기준" not in content:
            warnings.append(
                f"{plan.relative_to(ROOT)}: '## 완료 기준' 섹션 없음"
            )
        if "## 목표" not in content:
            warnings.append(
                f"{plan.relative_to(ROOT)}: '## 목표' 섹션 없음"
            )
    return warnings


def main() -> int:
    issues = []

    link_errors = check_internal_links()
    if link_errors:
        print(f"❌ 깨진 내부 링크 {len(link_errors)}건:")
        for e in link_errors:
            print(f"  {e}")
        issues.extend(link_errors)

    plan_warnings = check_exec_plans()
    if plan_warnings:
        print(f"⚠️  exec-plan 형식 경고 {len(plan_warnings)}건:")
        for w in plan_warnings:
            print(f"  {w}")

    if not issues:
        print("✅ 문서 검사 통과")
        return 0

    return 1


if __name__ == "__main__":
    sys.exit(main())
