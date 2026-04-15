#!/usr/bin/env python3
"""Find markdown lines that reference a target document basename."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path


DEFAULT_ROOT = Path(__file__).resolve().parent.parent


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=(
            "Recursively search markdown files for lines containing the basename "
            "of the provided markdown file."
        )
    )
    parser.add_argument(
        "target",
        help="Markdown file name or path to search references for.",
    )
    parser.add_argument(
        "--root",
        type=Path,
        default=DEFAULT_ROOT,
        help="Search root. Defaults to the repository root inferred from this script.",
    )
    return parser.parse_args()


def iter_markdown_files(root: Path) -> list[Path]:
    return sorted(path for path in root.rglob("*.md") if path.is_file())


def main() -> int:
    args = parse_args()
    root = args.root.resolve()

    if not root.is_dir():
        print(f"Search root is not a directory: {root}", file=sys.stderr)
        return 1

    target_basename = Path(args.target).name
    found = False

    for markdown_file in iter_markdown_files(root):
        with markdown_file.open("r", encoding="utf-8", errors="replace") as handle:
            for line_number, line in enumerate(handle, start=1):
                if target_basename in line:
                    relative_path = markdown_file.relative_to(root).as_posix()
                    print(f"{relative_path}:{line_number}  {line.strip()}")
                    found = True

    if not found:
        print("No references found.")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
