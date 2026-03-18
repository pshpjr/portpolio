#!/usr/bin/env python3
"""Check include dependency direction by top-level layers.

Usage:
    python tools/check_layers.py
    python tools/check_layers.py --root server/src --layer-order types,config,core,service,network,runtime
    python tools/check_layers.py --root Lib --layer-order include,src,tests
"""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path
from typing import TypedDict

DEFAULT_LAYER_ORDER = ["types", "config", "core", "service", "network", "runtime"]
INCLUDE_PATTERN = re.compile(r'#include\s+"([^"]+)"')
PREFIX_RE = re.compile(r"^\d+_")


class Violation(TypedDict):
    file: str
    line: int
    file_layer: str
    dep_layer: str
    include: str


def normalize_layer_name(name: str) -> str:
    return PREFIX_RE.sub("", name)


def parse_layer_order(raw: str) -> list[str]:
    layers = [normalize_layer_name(part.strip()) for part in raw.split(",") if part.strip()]
    if not layers:
        raise ValueError("layer order must not be empty")
    return layers


def get_layer(path: Path, root: Path, layer_rank: dict[str, int]) -> str | None:
    try:
        rel = path.relative_to(root)
    except ValueError:
        return None

    if not rel.parts:
        return None

    top = normalize_layer_name(rel.parts[0])
    return top if top in layer_rank else None


def get_include_layer(include_str: str, layer_rank: dict[str, int]) -> str | None:
    parts = Path(include_str).parts
    if not parts:
        return None
    top = normalize_layer_name(parts[0])
    return top if top in layer_rank else None


def check_file(file_path: Path, root: Path, layer_rank: dict[str, int]) -> list[Violation]:
    violations: list[Violation] = []
    file_layer = get_layer(file_path, root, layer_rank)
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
        dep_layer = get_include_layer(include_str, layer_rank)
        if dep_layer is None:
            continue

        if layer_rank[dep_layer] > layer_rank[file_layer]:
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


def iter_source_files(root: Path) -> list[Path]:
    files: list[Path] = []
    for ext in ("*.cpp", "*.h", "*.hpp"):
        files.extend(root.rglob(ext))
    return sorted(files)


def main() -> int:
    parser = argparse.ArgumentParser(description="Check include dependency rules.")
    parser.add_argument(
        "--root",
        "--src-root",
        dest="root",
        default="src",
        help="Directory whose top-level children are treated as layers (default: src)",
    )
    parser.add_argument(
        "--layer-order",
        default=",".join(DEFAULT_LAYER_ORDER),
        help="Comma-separated layer order from low-level to high-level",
    )
    args = parser.parse_args()

    root = Path(args.root)
    if not root.exists():
        print(f"error: root does not exist: {root}", file=sys.stderr)
        return 1

    try:
        layer_order = parse_layer_order(args.layer_order)
    except ValueError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1

    layer_rank = {layer: i for i, layer in enumerate(layer_order)}
    all_violations: list[Violation] = []
    for file_path in iter_source_files(root):
        all_violations.extend(check_file(file_path, root, layer_rank))

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

    print(f"Layer order: {' -> '.join(layer_order)}")
    print("Each layer may depend only on itself or earlier layers.")
    return 1


if __name__ == "__main__":
    sys.exit(main())
