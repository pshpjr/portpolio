#!/usr/bin/env python3
"""Skill / Agent 사용 로거 — Claude Code PostToolUse 훅으로 실행.

Skill 또는 Agent 도구 호출 시 _workspace/skill-usage-log.jsonl에 기록한다.

훅 stdin 형식 (Claude Code PostToolUse):
  {
    "session_id": "...",
    "tool_name": "Skill" | "Agent" | ...,
    "tool_input": { ... },
    "tool_response": { ... },
    "cwd": "/project/root"
  }

기록 대상:
  - tool_name == "Skill"  → skill 이름 (tool_input.skill)
  - tool_name == "Agent"  → subagent_type + description (tool_input)
"""

from __future__ import annotations

import json
import sys
from datetime import datetime, timezone
from pathlib import Path

LOG_FILENAME = "_workspace/skill-usage-log.jsonl"
TRACKED_TOOLS = {"Skill", "Agent"}


def build_entry(session_id: str, tool_name: str, tool_input: dict, now: datetime) -> dict:
    base = {
        "date": now.strftime("%Y-%m-%d"),
        "timestamp": now.isoformat(),
        "session_id": session_id,
        "tool": tool_name,
    }

    if tool_name == "Skill":
        base["name"] = tool_input.get("skill", "unknown")
        args = tool_input.get("args", "")
        if args:
            base["args"] = args[:120]  # 너무 길면 자름

    elif tool_name == "Agent":
        base["name"] = tool_input.get("subagent_type", "general-purpose")
        description = tool_input.get("description", "")
        if description:
            base["description"] = description[:120]

    return base


def main() -> int:
    raw = sys.stdin.read().strip()
    if not raw:
        return 0

    try:
        hook_data = json.loads(raw)
    except json.JSONDecodeError:
        return 0

    tool_name = hook_data.get("tool_name", "")
    if tool_name not in TRACKED_TOOLS:
        return 0

    session_id = hook_data.get("session_id", "unknown")
    tool_input = hook_data.get("tool_input", {})
    cwd = hook_data.get("cwd", ".")

    now = datetime.now(timezone.utc)
    entry = build_entry(session_id, tool_name, tool_input, now)

    log_path = Path(cwd) / LOG_FILENAME
    log_path.parent.mkdir(parents=True, exist_ok=True)

    with log_path.open("a", encoding="utf-8") as f:
        f.write(json.dumps(entry, ensure_ascii=False) + "\n")

    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except Exception as exc:  # pragma: no cover
        print(f"skill_usage_logger ERROR: {exc}", file=sys.stderr)
        raise SystemExit(0)  # 절대 세션을 막지 않는다
