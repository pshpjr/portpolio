#!/usr/bin/env python3
"""레이어 의존성 검사 툴.

src/ 디렉토리의 모든 C++ 파일에서 #include를 파싱하여
레이어 의존성 위반을 검사한다.

Usage:
    python3 tools/check_layers.py
    python3 tools/check_layers.py --src-root ./src

Exit codes:
    0: 위반 없음
    1: 위반 발견
"""

import sys
import re
import argparse
from pathlib import Path

# 레이어 순서 (왼쪽에서 오른쪽으로만 의존 가능)
LAYER_ORDER = ["types", "config", "core", "service", "network", "runtime"]
LAYER_RANK = {layer: i for i, layer in enumerate(LAYER_ORDER)}

INCLUDE_PATTERN = re.compile(r'#include\s+"([^"]+)"')


def get_layer(path: Path, src_root: Path) -> str | None:
    """파일 경로에서 레이어 이름을 추출한다."""
    try:
        rel = path.relative_to(src_root)
        top = rel.parts[0]
        return top if top in LAYER_RANK else None
    except ValueError:
        return None


def get_include_layer(include_str: str) -> str | None:
    """include 경로에서 레이어 이름을 추출한다."""
    parts = Path(include_str).parts
    if parts and parts[0] in LAYER_RANK:
        return parts[0]
    return None


def check_file(file_path: Path, src_root: Path) -> list[dict]:
    """단일 파일의 레이어 위반을 검사한다."""
    violations = []
    file_layer = get_layer(file_path, src_root)
    if file_layer is None:
        return violations

    try:
        content = file_path.read_text(encoding="utf-8")
    except (OSError, UnicodeDecodeError):
        return violations

    for lineno, line in enumerate(content.splitlines(), 1):
        match = INCLUDE_PATTERN.search(line)
        if not match:
            continue
        include_str = match.group(1)
        dep_layer = get_include_layer(include_str)
        if dep_layer is None:
            continue
        if LAYER_RANK[dep_layer] > LAYER_RANK[file_layer]:
            violations.append({
                "file": str(file_path),
                "line": lineno,
                "file_layer": file_layer,
                "dep_layer": dep_layer,
                "include": include_str,
            })
    return violations


def main() -> int:
    parser = argparse.ArgumentParser(description="레이어 의존성 검사")
    parser.add_argument(
        "--src-root",
        default="src",
        help="소스 루트 디렉토리 (default: src)",
    )
    args = parser.parse_args()

    src_root = Path(args.src_root)
    if not src_root.exists():
        print(f"오류: {src_root} 디렉토리가 없습니다.", file=sys.stderr)
        return 1

    all_violations = []
    for ext in ("*.cpp", "*.h", "*.hpp"):
        for file_path in src_root.rglob(ext):
            all_violations.extend(check_file(file_path, src_root))

    if not all_violations:
        print("✅ 레이어 위반 없음")
        return 0

    print(f"❌ 레이어 위반 {len(all_violations)}건 발견:\n")
    for v in all_violations:
        print(
            f"  {v['file']}:{v['line']}\n"
            f"    [{v['file_layer']}] → [{v['dep_layer']}] 위반\n"
            f"    #include \"{v['include']}\"\n"
        )
    print(f"\n레이어 순서: {' → '.join(LAYER_ORDER)}")
    print("각 레이어는 자신의 왼쪽 레이어만 include 가능합니다.")
    return 1


if __name__ == "__main__":
    sys.exit(main())
