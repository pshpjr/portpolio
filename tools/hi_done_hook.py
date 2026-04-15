#!/usr/bin/env python3
"""TaskUpdate(status=completed) 훅 — Claude Code PostToolUse에서 실행.

TaskUpdate가 completed 상태로 호출될 때, task 제목이 harness-improvement-queue의
항목 이름과 일치하면 hi_done.py를 자동 호출한다.

훅 stdin 형식:
  {
    "session_id": "...",
    "tool_name": "TaskUpdate",
    "tool_input": {"id": "...", "status": "completed", ...},
    "tool_response": {"type": "text", "text": "..."} | {"id": "...", "title": "...", ...},
    "cwd": "..."
  }
"""

from __future__ import annotations

import json
import re
import subprocess
import sys
from pathlib import Path

QUEUE_FILE = "docs/workflow/harness-improvement-queue.md"


def extract_title(hook_data: dict) -> str | None:
    """tool_response 또는 tool_input에서 task 제목을 추출한다."""
    response = hook_data.get("tool_response", {})

    # tool_response가 dict인 경우 title 필드 직접 확인
    if isinstance(response, dict):
        title = response.get("title") or response.get("name")
        if title:
            return title

    # tool_response가 텍스트(JSON 문자열)인 경우 파싱 시도
    if isinstance(response, dict) and response.get("type") == "text":
        try:
            parsed = json.loads(response.get("text", ""))
            title = parsed.get("title") or parsed.get("name")
            if title:
                return title
        except (json.JSONDecodeError, AttributeError):
            pass

    # tool_input에 title이 있는 경우 (일부 버전)
    tool_input = hook_data.get("tool_input", {})
    return tool_input.get("title") or tool_input.get("name")


def queue_short_titles(queue_path: Path) -> list[str]:
    """큐 파일에서 open 항목의 short-title 목록을 반환한다."""
    if not queue_path.exists():
        return []
    text = queue_path.read_text(encoding="utf-8")
    # '## Open Tasks' ~ '## Claimed' 사이만 파싱
    open_section = re.search(
        r"## Open Tasks(.*?)(?=## Claimed|## Done|$)", text, re.DOTALL
    )
    if not open_section:
        return []
    headings = re.findall(r"^## \d{4}-\d{2}-\d{2} - (.+)$", open_section.group(1), re.MULTILINE)
    return headings


def best_match(title: str, candidates: list[str]) -> str | None:
    """title의 단어들이 가장 많이 포함된 후보를 반환한다."""
    title_words = set(re.split(r"[\s\-_/]+", title.lower()))
    best, best_score = None, 0
    for candidate in candidates:
        cand_words = set(re.split(r"[\s\-_/]+", candidate.lower()))
        score = len(title_words & cand_words)
        if score > best_score:
            best, best_score = candidate, score
    # 최소 2단어 이상 겹쳐야 매칭으로 인정
    return best if best_score >= 2 else None


def main() -> int:
    raw = sys.stdin.read().strip()
    if not raw:
        return 0

    try:
        hook_data = json.loads(raw)
    except json.JSONDecodeError:
        return 0

    if hook_data.get("tool_name") != "TaskUpdate":
        return 0

    tool_input = hook_data.get("tool_input", {})
    if tool_input.get("status") != "completed":
        return 0

    title = extract_title(hook_data)
    if not title:
        return 0

    cwd = hook_data.get("cwd", ".")
    queue_path = Path(cwd) / QUEUE_FILE
    candidates = queue_short_titles(queue_path)
    if not candidates:
        return 0

    matched = best_match(title, candidates)
    if not matched:
        return 0

    hi_done = Path(cwd) / "tools/hi_done.py"
    result = subprocess.run(
        [sys.executable, str(hi_done), matched],
        capture_output=True,
        text=True,
        cwd=cwd,
    )
    if result.stdout:
        print(result.stdout, file=sys.stderr)

    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except Exception as exc:
        print(f"hi_done_hook ERROR: {exc}", file=sys.stderr)
        raise SystemExit(0)  # 절대 세션을 막지 않는다
