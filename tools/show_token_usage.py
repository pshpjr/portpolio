#!/usr/bin/env python3
"""Display token usage summary from _workspace/token-log.jsonl.

Shows per-session stats and daily/total aggregates.

Usage:
    python tools/show_token_usage.py
    python tools/show_token_usage.py --days 7
    python tools/show_token_usage.py --session <session_id_prefix>
"""

from __future__ import annotations

import argparse
import json
from collections import defaultdict
from datetime import date, timedelta
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[1]
LOG_PATH = REPO_ROOT / "_workspace" / "token-log.jsonl"


def load_entries(days: int | None = None, session_filter: str = "") -> list[dict]:
    if not LOG_PATH.exists():
        return []

    cutoff = (date.today() - timedelta(days=days)).isoformat() if days else None
    entries: list[dict] = []

    with LOG_PATH.open(encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            try:
                entry = json.loads(line)
            except json.JSONDecodeError:
                continue
            if cutoff and entry.get("date", "") < cutoff:
                continue
            if session_filter and session_filter not in entry.get("session_id", ""):
                continue
            entries.append(entry)

    return entries


def latest_per_session(entries: list[dict]) -> list[dict]:
    """Keep only the latest entry per session_id (max turns = most complete)."""
    by_session: dict[str, dict] = {}
    for entry in entries:
        sid = entry.get("session_id", "?")
        existing = by_session.get(sid)
        if existing is None or entry.get("turns", 0) >= existing.get("turns", 0):
            by_session[sid] = entry
    return sorted(by_session.values(), key=lambda e: e.get("timestamp", ""), reverse=True)


def fmt_tokens(n: int) -> str:
    if n >= 1_000_000:
        return f"{n/1_000_000:.2f}M"
    if n >= 1_000:
        return f"{n/1_000:.1f}k"
    return str(n)


def print_session_table(sessions: list[dict]) -> None:
    if not sessions:
        print("  (no sessions)")
        return

    hdr = f"{'Date':<12} {'Session':<10} {'Turns':>5} {'Input':>8} {'Output':>8} {'CacheCreate':>12} {'CacheRead':>12} {'Billed':>8}"
    print(hdr)
    print("-" * len(hdr))
    for s in sessions:
        sid = s.get("session_id", "?")[:8]
        print(
            f"{s.get('date','?'):<12} {sid:<10} {s.get('turns',0):>5} "
            f"{fmt_tokens(s.get('input_tokens',0)):>8} "
            f"{fmt_tokens(s.get('output_tokens',0)):>8} "
            f"{fmt_tokens(s.get('cache_creation_input_tokens',0)):>12} "
            f"{fmt_tokens(s.get('cache_read_input_tokens',0)):>12} "
            f"{fmt_tokens(s.get('total_billed_tokens',0)):>8}"
        )


def print_daily_summary(sessions: list[dict]) -> None:
    by_date: dict[str, dict] = defaultdict(lambda: {
        "sessions": 0,
        "turns": 0,
        "input_tokens": 0,
        "output_tokens": 0,
        "cache_creation_input_tokens": 0,
        "cache_read_input_tokens": 0,
        "total_billed_tokens": 0,
    })
    for s in sessions:
        d = s.get("date", "?")
        by_date[d]["sessions"] += 1
        for key in ("turns", "input_tokens", "output_tokens",
                    "cache_creation_input_tokens", "cache_read_input_tokens",
                    "total_billed_tokens"):
            by_date[d][key] += s.get(key, 0)

    hdr = f"{'Date':<12} {'Sessions':>8} {'Turns':>6} {'Input':>8} {'Output':>8} {'CacheCreate':>12} {'CacheRead':>12} {'Billed':>8}"
    print(hdr)
    print("-" * len(hdr))
    for d in sorted(by_date.keys(), reverse=True):
        row = by_date[d]
        print(
            f"{d:<12} {row['sessions']:>8} {row['turns']:>6} "
            f"{fmt_tokens(row['input_tokens']):>8} "
            f"{fmt_tokens(row['output_tokens']):>8} "
            f"{fmt_tokens(row['cache_creation_input_tokens']):>12} "
            f"{fmt_tokens(row['cache_read_input_tokens']):>12} "
            f"{fmt_tokens(row['total_billed_tokens']):>8}"
        )


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Display token usage summary from _workspace/token-log.jsonl."
    )
    parser.add_argument("--days", type=int, default=None, help="Filter to last N days.")
    parser.add_argument("--session", default="", help="Filter by session ID prefix.")
    parser.add_argument("--raw", action="store_true", help="Show raw entries (all checkpoints per session).")
    args = parser.parse_args()

    if not LOG_PATH.exists():
        print(f"No token log found at {LOG_PATH.relative_to(REPO_ROOT)}")
        print("Make sure the Stop hook is configured and at least one session has completed.")
        return 0

    entries = load_entries(days=args.days, session_filter=args.session)
    if not entries:
        print("No matching entries.")
        return 0

    if args.raw:
        print(f"\nRaw entries ({len(entries)} total):")
        print_session_table(entries)
        return 0

    sessions = latest_per_session(entries)

    print(f"\n=== Token Usage — {LOG_PATH.relative_to(REPO_ROOT)} ===")
    print(f"Sessions: {len(sessions)}")
    print()

    print("Per-session (latest snapshot):")
    print_session_table(sessions)

    print()
    print("Daily summary (deduplicated):")
    print_daily_summary(sessions)

    # Overall totals
    total_billed = sum(s.get("total_billed_tokens", 0) for s in sessions)
    total_cache_read = sum(s.get("cache_read_input_tokens", 0) for s in sessions)
    total_cache_create = sum(s.get("cache_creation_input_tokens", 0) for s in sessions)
    total_turns = sum(s.get("turns", 0) for s in sessions)
    print()
    print(f"Totals across {len(sessions)} session(s):")
    print(f"  Billed tokens:        {fmt_tokens(total_billed)}")
    print(f"  Cache read tokens:    {fmt_tokens(total_cache_read)}  (KV cache hits — not billed at full rate)")
    print(f"  Cache create tokens:  {fmt_tokens(total_cache_create)}")
    print(f"  Total turns:          {total_turns}")

    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except Exception as exc:  # pragma: no cover
        print(f"ERROR: {exc}")
        raise SystemExit(1)
