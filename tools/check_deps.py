#!/usr/bin/env python3
"""Detect include cycles between implementation files.

Warns when two or more .cpp files include each other's headers,
forming a mutual or cyclic dependency.

Usage:
    python tools/check_deps.py
    python tools/check_deps.py --root server/src
"""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

INCLUDE_PATTERN = __import__("re").compile(r'#include\s+"([^"]+)"')


def build_header_to_cpp_map(root: Path) -> dict[str, Path]:
    """Map header filename ('item.h') → its paired .cpp path."""
    result: dict[str, Path] = {}
    for cpp in root.rglob("*.cpp"):
        h = cpp.with_suffix(".h")
        if h.exists():
            result[h.name] = cpp
    return result


def build_module_graph(
    root: Path, header_to_cpp: dict[str, Path]
) -> dict[Path, list[Path]]:
    """Build directed graph: cpp → list of cpp files it depends on via headers."""
    cpps = list(root.rglob("*.cpp"))
    graph: dict[Path, list[Path]] = {cpp: [] for cpp in cpps}
    for cpp in cpps:
        try:
            content = cpp.read_text(encoding="utf-8")
        except (OSError, UnicodeDecodeError):
            continue
        for match in INCLUDE_PATTERN.finditer(content):
            header_name = Path(match.group(1)).name
            dep_cpp = header_to_cpp.get(header_name)
            if dep_cpp and dep_cpp != cpp:
                graph[cpp].append(dep_cpp)
    return graph


def find_cycles(graph: dict[Path, list[Path]]) -> list[list[Path]]:
    """Return list of cycles; each cycle is a list of cpp file paths."""
    WHITE, GRAY, BLACK = 0, 1, 2
    color: dict[Path, int] = {node: WHITE for node in graph}
    stack: list[Path] = []
    cycles: list[list[Path]] = []
    seen: set[frozenset[Path]] = set()

    def dfs(node: Path) -> None:
        color[node] = GRAY
        stack.append(node)
        for neighbor in graph.get(node, []):
            if neighbor not in color:
                color[neighbor] = WHITE
            if color[neighbor] == GRAY:
                idx = stack.index(neighbor)
                cycle = list(stack[idx:])
                key = frozenset(cycle)
                if key not in seen:
                    seen.add(key)
                    cycles.append(cycle)
            elif color[neighbor] == WHITE:
                dfs(neighbor)
        stack.pop()
        color[node] = BLACK

    for node in list(graph.keys()):
        if color.get(node, WHITE) == WHITE:
            dfs(node)

    return cycles


def main() -> int:
    parser = argparse.ArgumentParser(description="Detect include cycles between implementation files.")
    parser.add_argument(
        "--root",
        "--src-root",
        dest="root",
        default="src",
        help="Source root to scan (default: src)",
    )
    args = parser.parse_args()

    root = Path(args.root)
    if not root.exists():
        print(f"error: root does not exist: {root}", file=sys.stderr)
        return 1

    header_to_cpp = build_header_to_cpp_map(root)
    graph = build_module_graph(root, header_to_cpp)
    cycles = find_cycles(graph)

    if cycles:
        print(f"WARN: found {len(cycles)} include cycle(s) between implementation files.\n")
        for cycle in cycles:
            names = [p.stem for p in cycle]
            chain = " -> ".join(names) + f" -> {names[0]}"
            print(f"  cycle: {chain}")
            for cpp in cycle:
                print(f"    {cpp}")
        print()
    else:
        print("OK: no module include cycles found.")

    return 0


if __name__ == "__main__":
    sys.exit(main())
