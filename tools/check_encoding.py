#!/usr/bin/env python3
"""Validate repository text file encodings and detect likely mojibake.

Usage:
    python tools/check_encoding.py
    python tools/check_encoding.py --root .

Exit codes:
    0: all checked files are valid UTF-8 without BOM and show no mojibake
    1: one or more files are invalid UTF-8, contain a UTF-8 BOM, or look corrupted
"""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path

TEXT_SUFFIXES = {
    ".md",
    ".py",
    ".txt",
    ".json",
    ".yml",
    ".yaml",
    ".toml",
    ".cmake",
    ".cpp",
    ".h",
    ".hpp",
    ".c",
    ".cc",
}

MOJIBAKE_TEXT_SUFFIXES = {
    ".md",
    ".py",
    ".txt",
    ".json",
    ".yml",
    ".yaml",
    ".toml",
    ".cmake",
}

SUSPICIOUS_CJK_RE = re.compile(r"[\u3400-\u4dbf\u4e00-\u9fff\uf900-\ufaff]")
REPLACEMENT_CHAR = "\ufffd"

TEXT_FILENAMES = {
    "CMakeLists.txt",
    ".clang-format",
    ".gitignore",
}

SKIP_DIR_NAMES = {
    ".git",
    "build",
    ".vcpkg-downloads",
    ".vcpkg-registries",
    ".vcpkg-bincache",
    ".localappdata",
    ".appdata",
}


def should_check(path: Path) -> bool:
    if not path.is_file():
        return False
    if any(part in SKIP_DIR_NAMES for part in path.parts):
        return False
    return path.suffix.lower() in TEXT_SUFFIXES or path.name in TEXT_FILENAMES


def check_utf8(path: Path) -> str | None:
    data = path.read_bytes()
    if data.startswith(b"\xef\xbb\xbf"):
        return "contains UTF-8 BOM"

    try:
        data.decode("utf-8")
    except UnicodeDecodeError as exc:
        return f"invalid UTF-8 at byte {exc.start}"

    return None


def looks_like_mojibake(path: Path, text: str) -> list[str]:
    """Return suspicious line reports for likely mojibake content.

    This is heuristic-based. It targets common corruption patterns seen when
    Korean UTF-8 text is decoded with the wrong code page and then saved again.
    """
    if path.suffix.lower() not in MOJIBAKE_TEXT_SUFFIXES:
        return []

    issues: list[str] = []
    for line_no, line in enumerate(text.splitlines(), start=1):
        if REPLACEMENT_CHAR in line:
            issues.append(f"{path}: line {line_no}: contains replacement character")
            continue

        question_count = line.count("?")
        suspicious_cjk_count = len(SUSPICIOUS_CJK_RE.findall(line))

        if question_count >= 2 and suspicious_cjk_count >= 2:
            issues.append(f"{path}: line {line_no}: likely mojibake content")
            continue

        if "??" in line and suspicious_cjk_count >= 1:
            issues.append(f"{path}: line {line_no}: likely mojibake content")

    return issues


def main() -> int:
    parser = argparse.ArgumentParser(description="Check repository text files are UTF-8.")
    parser.add_argument(
        "--root",
        default=".",
        help="Repository root to scan (default: current directory)",
    )
    args = parser.parse_args()

    root = Path(args.root).resolve()
    if not root.exists():
        print(f"error: root does not exist: {root}", file=sys.stderr)
        return 1

    errors: list[str] = []
    for path in sorted(root.rglob("*")):
        if not should_check(path):
            continue

        issue = check_utf8(path)
        if issue is not None:
            errors.append(f"{path.relative_to(root)}: {issue}")
            continue

        text = path.read_text(encoding="utf-8")
        rel = path.relative_to(root)
        for issue in looks_like_mojibake(rel, text):
            errors.append(issue)

    if errors:
        print(f"ERROR: found {len(errors)} encoding issue(s).")
        for error in errors:
            print(f"  {error}")
        print("Policy: repository text files must be UTF-8 without BOM and must not contain mojibake.")
        return 1

    print("OK: all checked text files are UTF-8 without BOM and look readable.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
