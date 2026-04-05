#!/usr/bin/env python3
"""Estimate context-token load against current work size."""

from __future__ import annotations

import argparse
import math
import subprocess
import sys
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[1]


def estimate_tokens(text: str) -> int:
    # Rough cross-model heuristic: 1 token ~= 4 characters.
    return math.ceil(len(text) / 4)


def read_file(path_str: str) -> tuple[str, int, int, int]:
    path = (REPO_ROOT / path_str).resolve()
    if not path.exists():
        raise FileNotFoundError(f"missing file: {path_str}")
    text = path.read_text(encoding="utf-8")
    return path_str, len(text.splitlines()), len(text), estimate_tokens(text)


def git_diff_stats(git_base: str) -> tuple[int, int, int]:
    cmd = ["git", "diff", "--numstat", git_base, "--"]
    result = subprocess.run(
        cmd,
        cwd=REPO_ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    if result.returncode != 0:
        raise RuntimeError(result.stderr.strip() or "git diff failed")

    changed_files = 0
    added = 0
    deleted = 0
    for line in result.stdout.splitlines():
        parts = line.split("\t")
        if len(parts) < 3:
            continue
        add_str, del_str = parts[0], parts[1]
        if add_str == "-" or del_str == "-":
            continue
        changed_files += 1
        added += int(add_str)
        deleted += int(del_str)
    untracked_cmd = ["git", "ls-files", "--others", "--exclude-standard"]
    untracked = subprocess.run(
        untracked_cmd,
        cwd=REPO_ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    if untracked.returncode != 0:
        raise RuntimeError(untracked.stderr.strip() or "git ls-files failed")

    for rel_path in untracked.stdout.splitlines():
        rel_path = rel_path.strip()
        if not rel_path:
            continue
        abs_path = REPO_ROOT / rel_path
        if not abs_path.is_file():
            continue
        changed_files += 1
        added += len(abs_path.read_text(encoding="utf-8").splitlines())

    return changed_files, added, deleted


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Estimate context-token load against changed-line work size."
    )
    parser.add_argument(
        "--files",
        nargs="*",
        default=[],
        help="Repository-relative files read as task context.",
    )
    parser.add_argument(
        "--git-base",
        default="HEAD",
        help="Git base for diff stats. Defaults to HEAD.",
    )
    args = parser.parse_args()

    file_rows: list[tuple[str, int, int, int]] = []
    for file_path in args.files:
        file_rows.append(read_file(file_path))

    total_lines = sum(row[1] for row in file_rows)
    total_chars = sum(row[2] for row in file_rows)
    total_tokens = sum(row[3] for row in file_rows)

    changed_files, added_lines, deleted_lines = git_diff_stats(args.git_base)
    changed_lines = added_lines + deleted_lines
    tokens_per_changed_line = (
        round(total_tokens / changed_lines, 2) if changed_lines else None
    )

    print("Context Meter")
    print(f"- git_base: {args.git_base}")
    print(f"- context_files: {len(file_rows)}")
    print(f"- context_lines: {total_lines}")
    print(f"- context_chars: {total_chars}")
    print(f"- estimated_context_tokens: {total_tokens}")
    print(f"- changed_files: {changed_files}")
    print(f"- changed_lines: {changed_lines}")
    print(f"- added_lines: {added_lines}")
    print(f"- deleted_lines: {deleted_lines}")
    if tokens_per_changed_line is None:
        print("- estimated_tokens_per_changed_line: n/a")
    else:
        print(f"- estimated_tokens_per_changed_line: {tokens_per_changed_line}")

    if file_rows:
        print("")
        print("Per-file")
        for path_str, line_count, char_count, token_count in file_rows:
            print(
                f"- {path_str}: lines={line_count}, chars={char_count}, estimated_tokens={token_count}"
            )

    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except Exception as exc:  # pragma: no cover - CLI failure path
        print(f"ERROR: {exc}", file=sys.stderr)
        raise SystemExit(1)
