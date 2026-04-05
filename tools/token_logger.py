#!/usr/bin/env python3
"""Automatic token usage logger — designed to run as a Claude Code Stop hook.

Called with hook JSON on stdin (from Claude Code Stop event):
  {
    "session_id": "...",
    "transcript_path": "/path/to/session.jsonl",
    "cwd": "/project/root",
    "hook_event_name": "Stop",
    ...
  }

Reads the session transcript, sums usage across all assistant messages,
and appends a cumulative entry to _workspace/token-log.jsonl.

Usage (manual test):
    echo '{"session_id":"test","transcript_path":"~/.claude/projects/.../session.jsonl","cwd":"."}' | python tools/token_logger.py
"""

from __future__ import annotations

import json
import sys
from datetime import datetime, timezone
from pathlib import Path


LOG_FILENAME = "_workspace/token-log.jsonl"


def read_transcript_usage(transcript_path: str) -> dict:
    """Sum all assistant message usage fields from a JSONL transcript."""
    path = Path(transcript_path).expanduser()
    if not path.exists():
        raise FileNotFoundError(f"transcript not found: {transcript_path}")

    totals: dict[str, int] = {
        "input_tokens": 0,
        "output_tokens": 0,
        "cache_creation_input_tokens": 0,
        "cache_read_input_tokens": 0,
    }
    models_seen: set[str] = set()
    turn_count = 0

    with path.open(encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            try:
                obj = json.loads(line)
            except json.JSONDecodeError:
                continue

            if obj.get("type") != "assistant":
                continue

            turn_count += 1
            msg = obj.get("message", {})
            usage = msg.get("usage", {})
            model = msg.get("model", "unknown")
            if model:
                models_seen.add(model)

            totals["input_tokens"] += usage.get("input_tokens", 0)
            totals["output_tokens"] += usage.get("output_tokens", 0)
            totals["cache_creation_input_tokens"] += usage.get("cache_creation_input_tokens", 0)
            totals["cache_read_input_tokens"] += usage.get("cache_read_input_tokens", 0)

    return {
        **totals,
        "turns": turn_count,
        "models": sorted(models_seen),
    }


def main() -> int:
    raw = sys.stdin.read().strip()
    if not raw:
        print("token_logger: no stdin data, skipping", file=sys.stderr)
        return 0

    try:
        hook_data = json.loads(raw)
    except json.JSONDecodeError as exc:
        print(f"token_logger: invalid JSON on stdin: {exc}", file=sys.stderr)
        return 0

    session_id = hook_data.get("session_id", "unknown")
    transcript_path = hook_data.get("transcript_path", "")
    cwd = hook_data.get("cwd", ".")

    if not transcript_path:
        print("token_logger: no transcript_path in hook data", file=sys.stderr)
        return 0

    try:
        usage = read_transcript_usage(transcript_path)
    except Exception as exc:
        print(f"token_logger: failed to read transcript: {exc}", file=sys.stderr)
        return 0

    log_path = Path(cwd) / LOG_FILENAME
    log_path.parent.mkdir(parents=True, exist_ok=True)

    now = datetime.now(timezone.utc)
    entry = {
        "date": now.strftime("%Y-%m-%d"),
        "timestamp": now.isoformat(),
        "session_id": session_id,
        "models": usage["models"],
        "turns": usage["turns"],
        "input_tokens": usage["input_tokens"],
        "output_tokens": usage["output_tokens"],
        "cache_creation_input_tokens": usage["cache_creation_input_tokens"],
        "cache_read_input_tokens": usage["cache_read_input_tokens"],
        "total_billed_tokens": usage["input_tokens"] + usage["output_tokens"],
    }

    with log_path.open("a", encoding="utf-8") as f:
        f.write(json.dumps(entry, ensure_ascii=False) + "\n")

    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except Exception as exc:  # pragma: no cover
        print(f"token_logger ERROR: {exc}", file=sys.stderr)
        raise SystemExit(0)  # Never block the session
