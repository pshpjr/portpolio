#!/usr/bin/env python3
"""Check layer dependency violations in src/.

Scans C++ source/header files for project-local #include directives and reports
when a layer depends on a later layer.

Usage:
    python tools/check_layers.py
    python tools/check_layers.py --src-root ./src

Exit codes:
    0: no violations found
    1: violations found or invalid input
"""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path
from typing import TypedDict

LAYER_ORDER = ["types", "config", "core", "service", "network", "runtime"]
LAYER_RANK = {layer: i for i, layer in enumerate(LAYER_ORDER)}
INCLUDE_PATTERN = re.compile(r'#include\s+"([^"]+)"')
_PREFIX_RE = re.compile(r'^\d+_')


def _strip_prefix(name: str) -> str:
    """Strip leading numeric prefix (e.g. '01_types' -> 'types')."""
    return _PREFIX_RE.sub("", name)


class Violation(TypedDict):
    file: str
    line: int
    file_layer: str
    dep_layer: str
    include: str


def get_layer(path: Path, src_root: Path) -> str | None:
    """Return the top-level layer name for a source file."""
    try:
        rel = path.relative_to(src_root)
    except ValueError:
        return None

    top = rel.parts[0]
    normalized = _strip_prefix(top)
    return normalized if normalized in LAYER_RANK else None


def get_include_layer(include_str: str) -> str | None:
    """Return the referenced top-level layer name for an include path."""
    parts = Path(include_str).parts
    if parts:
        normalized = _strip_prefix(parts[0])
        if normalized in LAYER_RANK:
            return normalized
    return None


def check_file(file_path: Path, src_root: Path) -> list[Violation]:
    """Check a single file for forward layer dependencies."""
    violations: list[Violation] = []
    file_layer = get_layer(file_path, src_root)
    if file_layer is None:
        return violations

    try:
        content = file_path.read_text(encoding="utf-8")
    except (OSError, UnicodeDecodeError):
        return violations

    for lineno, line in enumerate(content.splitlines(), start=1):
        match = INCLUDE_PATTERN.search(line)
        if not match:
            continue

        include_str = match.group(1)
        dep_layer = get_include_layer(include_str)
        if dep_layer is None:
            continue

        if LAYER_RANK[dep_layer] > LAYER_RANK[file_layer]:
            violations.append(
                {
                    "file": str(file_path),
                    "line": lineno,
                    "file_layer": file_layer,
                    "dep_layer": dep_layer,
                    "include": include_str,
                }
            )

    return violations


def main() -> int:
    parser = argparse.ArgumentParser(description="Check layer dependency rules.")
    parser.add_argument(
        "--src-root",
        default="src",
        help="Source root directory (default: src)",
    )
    args = parser.parse_args()

    src_root = Path(args.src_root)
    if not src_root.exists():
        print(f"error: source root does not exist: {src_root}", file=sys.stderr)
        return 1

    all_violations: list[Violation] = []
    for ext in ("*.cpp", "*.h", "*.hpp"):
        for file_path in src_root.rglob(ext):
            all_violations.extend(check_file(file_path, src_root))

    if not all_violations:
        print("OK: no layer violations found.")
        return 0

    print(f"ERROR: found {len(all_violations)} layer violation(s).\n")
    for violation in all_violations:
        print(
            f"  {violation['file']}:{violation['line']}\n"
            f"    [{violation['file_layer']}] -> [{violation['dep_layer']}]\n"
            f"    #include \"{violation['include']}\"\n"
        )

    print(f"Layer order: {' -> '.join(LAYER_ORDER)}")
    print("Each layer may depend only on itself or earlier layers.")
    return 1


if __name__ == "__main__":
    sys.exit(main())
