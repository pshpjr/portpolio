#!/usr/bin/env python3
"""find_doc_refs.py — 마크다운 파일이 저장소 어디서 참조되는지 출력한다.

Usage:
    python tools/find_doc_refs.py <파일경로>
    python tools/find_doc_refs.py docs/workflow/harness-improvement-queue.md

출력:
    file_path:line_number  (참조하는 줄)
"""

from __future__ import annotations

import re
import subprocess
import sys
from pathlib import Path


def main() -> int:
    if len(sys.argv) < 2:
        print("Usage: python tools/find_doc_refs.py <파일경로>", file=sys.stderr)
        return 1

    target = Path(sys.argv[1])
    # 파일명만, 확장자 포함
    filename = target.name
    # 확장자 없는 이름도 검색 패턴에 포함 (anchor 링크 등)
    stem = target.stem

    # ripgrep으로 저장소 전체 검색
    # 패턴: 파일명 또는 경로 조각이 포함된 줄
    pattern = re.escape(filename)

    result = subprocess.run(
        [
            "rg",
            "--line-number",
            "--no-heading",
            "--color", "never",
            "-g", "*.md",
            "-g", "*.json",
            "-g", "*.yaml",
            "-g", "*.yml",
            "-g", "*.toml",
            pattern,
        ],
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
    )

    lines = [l for l in result.stdout.splitlines() if l.strip()]

    # 자기 자신 제외
    target_str = str(target).replace("\\", "/")
    filtered = [l for l in lines if not l.startswith(target_str.lstrip("./")+":") and target.name not in Path(l.split(":")[0]).name + ":__self__"]

    # 자기 자신 경로 정규화해서 제외
    def is_self(line: str) -> bool:
        parts = line.split(":", 1)
        if not parts:
            return False
        ref_path = Path(parts[0]).as_posix()
        tgt_path = target.as_posix().lstrip("./")
        return ref_path.endswith(tgt_path) or tgt_path.endswith(ref_path)

    filtered = [l for l in lines if not is_self(l)]

    if not filtered:
        print(f"참조 없음: {filename}")
        return 0

    print(f"[{filename}] 참조 {len(filtered)}건:")
    for line in filtered:
        print(" ", line)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
