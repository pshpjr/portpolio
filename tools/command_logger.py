#!/usr/bin/env python3
"""command_logger.py — Bash 도구 호출 목적과 커맨드를 로깅하는 PostToolUse 훅.

Claude Code PostToolUse 훅으로 실행되며, Bash 도구 호출 시
_workspace/command-log.jsonl에 구조화된 기록을 남긴다.

훅 stdin 형식 (Claude Code PostToolUse):
  {
    "session_id": "...",
    "tool_name": "Bash",
    "tool_input": {
      "command": "python tools/foo.py ...",
      "description": "목적 설명"
    },
    "tool_response": { ... },
    "cwd": "/project/root"
  }

기록 필드:
  - timestamp, session_id
  - command   : 실행된 커맨드 (최대 300자)
  - purpose   : tool_input.description (에이전트가 기록한 목적)
  - script    : 커맨드에서 추출한 주요 스크립트 경로 (py/sh/ps1 파일)
  - cwd
  - exit_code : tool_response에서 추출 (있을 때만)
"""

from __future__ import annotations

import json
import re
import sys
from datetime import datetime, timezone
from pathlib import Path

LOG_FILENAME = "_workspace/command-log.jsonl"
SCRIPT_PATTERN = re.compile(r"[\w./\\-]+\.(?:py|sh|ps1|bat|cmd)\b")


def extract_script(command: str) -> str | None:
    """커맨드 문자열에서 첫 번째 스크립트 파일 경로를 추출한다."""
    m = SCRIPT_PATTERN.search(command)
    return m.group(0) if m else None


def extract_exit_code(tool_response: dict) -> int | None:
    """tool_response에서 exit code를 추출한다 (있을 때만)."""
    # Claude Code response 구조에 따라 다를 수 있음
    if isinstance(tool_response, dict):
        code = tool_response.get("exit_code") or tool_response.get("returncode")
        if code is not None:
            try:
                return int(code)
            except (TypeError, ValueError):
                pass
    return None


def main() -> int:
    raw = sys.stdin.read().strip()
    if not raw:
        return 0

    try:
        hook_data = json.loads(raw)
    except json.JSONDecodeError:
        return 0

    if hook_data.get("tool_name") != "Bash":
        return 0

    session_id = hook_data.get("session_id", "unknown")
    tool_input = hook_data.get("tool_input", {})
    tool_response = hook_data.get("tool_response", {})
    cwd = hook_data.get("cwd", ".")

    command: str = tool_input.get("command", "")
    purpose: str = tool_input.get("description", "")

    if not command:
        return 0

    now = datetime.now(timezone.utc)
    entry: dict = {
        "date": now.strftime("%Y-%m-%d"),
        "timestamp": now.isoformat(),
        "session_id": session_id,
        "command": command[:300],
        "purpose": purpose[:200] if purpose else None,
        "script": extract_script(command),
        "cwd": cwd,
    }

    exit_code = extract_exit_code(tool_response)
    if exit_code is not None:
        entry["exit_code"] = exit_code

    # None 값 제거
    entry = {k: v for k, v in entry.items() if v is not None}

    log_path = Path(cwd) / LOG_FILENAME
    log_path.parent.mkdir(parents=True, exist_ok=True)

    with log_path.open("a", encoding="utf-8") as f:
        f.write(json.dumps(entry, ensure_ascii=False) + "\n")

    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except Exception as exc:  # pragma: no cover
        print(f"command_logger ERROR: {exc}", file=sys.stderr)
        raise SystemExit(0)  # 절대 세션을 막지 않는다
