#!/usr/bin/env python3
"""
harness-improvement-queue.md에서 항목을 완료 처리하는 스크립트.

사용법:
    python tools/hi_done.py <short-title> [--resolution "완료 메모"]

예시:
    python tools/hi_done.py add-codex-review-skill
    python tools/hi_done.py add-codex-review-skill --resolution "SKILL.md 추가 완료"
"""

import argparse
import re
import sys
from datetime import date
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent
QUEUE_FILE = REPO_ROOT / "docs/workflow/harness-improvement-queue.md"
DONE_FILE = REPO_ROOT / "docs/workflow/harness-improvement-done.md"


def parse_blocks(text: str) -> list[tuple[str, str]]:
    """텍스트를 (heading, body) 쌍의 블록으로 분리한다."""
    # '## YYYY-MM-DD - ...' 패턴으로 항목 경계를 찾는다
    pattern = re.compile(r"(?=^## \d{4}-\d{2}-\d{2} - )", re.MULTILINE)
    parts = pattern.split(text)
    blocks = []
    for part in parts:
        if not part.strip():
            continue
        heading_match = re.match(r"^(## \S.*?)$", part, re.MULTILINE)
        if heading_match:
            blocks.append((heading_match.group(1), part))
        else:
            blocks.append(("", part))
    return blocks


def find_block(blocks: list[tuple[str, str]], name: str) -> int:
    """short-title 또는 전체 heading으로 블록 인덱스를 반환한다."""
    name_lower = name.lower()
    for i, (heading, _) in enumerate(blocks):
        if name_lower in heading.lower():
            return i
    return -1


def set_status(block_text: str, new_status: str) -> str:
    return re.sub(r"(- status:\s*)\S+", rf"\g<1>{new_status}", block_text, count=1)


def append_resolution(block_text: str, resolution: str) -> str:
    today = date.today().isoformat()
    line = f"- resolution: {resolution} {today} 처리 완료."
    # resolution 필드가 이미 있으면 교체, 없으면 source 뒤에 추가
    if re.search(r"^- resolution:", block_text, re.MULTILINE):
        return re.sub(r"^- resolution:.*$", line, block_text, flags=re.MULTILINE)
    # source 줄 뒤에 삽입
    source_match = re.search(r"^(- source:.*)$", block_text, re.MULTILINE)
    if source_match:
        end = source_match.end()
        return block_text[:end] + "\n" + line + block_text[end:]
    return block_text.rstrip() + "\n" + line + "\n"


def rebuild_queue(pre_item: str, blocks: list[tuple[str, str]], removed_idx: int) -> str:
    remaining = [body for i, (_, body) in enumerate(blocks) if i != removed_idx]
    return pre_item + "".join(remaining)


def append_to_done(done_path: Path, block_text: str) -> None:
    separator = "\n---\n\n"
    existing = done_path.read_text(encoding="utf-8") if done_path.exists() else ""
    # 마지막 줄바꿈 정리 후 구분선과 항목 추가
    content = existing.rstrip() + "\n" + separator + block_text.strip() + "\n"
    done_path.write_text(content, encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser(description="harness-improvement 항목을 완료 처리한다")
    parser.add_argument("name", help="완료할 항목의 short-title (부분 일치 가능)")
    parser.add_argument("--resolution", default="", help="완료 메모 (선택)")
    args = parser.parse_args()

    if not QUEUE_FILE.exists():
        print(f"오류: 큐 파일을 찾을 수 없습니다: {QUEUE_FILE}", file=sys.stderr)
        sys.exit(1)

    queue_text = QUEUE_FILE.read_text(encoding="utf-8")

    # '## Open Tasks' 이전의 헤더/설명 부분과 이후의 항목 부분을 분리
    split_marker = "## Open Tasks"
    split_pos = queue_text.find(split_marker)
    if split_pos == -1:
        print("오류: '## Open Tasks' 섹션을 찾을 수 없습니다.", file=sys.stderr)
        sys.exit(1)

    pre_section = queue_text[: split_pos + len(split_marker) + 1]  # '## Open Tasks\n' 포함
    items_section = queue_text[split_pos + len(split_marker) + 1 :]

    # items_section을 항목 블록과 나머지(Claimed/Done 섹션)로 분리
    tail_marker = re.search(r"^## (Claimed|Done)", items_section, re.MULTILINE)
    if tail_marker:
        items_text = items_section[: tail_marker.start()]
        tail_text = items_section[tail_marker.start() :]
    else:
        items_text = items_section
        tail_text = ""

    blocks = parse_blocks(items_text)
    idx = find_block(blocks, args.name)

    if idx == -1:
        print(f"오류: '{args.name}'과 일치하는 항목을 찾을 수 없습니다.", file=sys.stderr)
        print("\n현재 open 항목:")
        for heading, _ in blocks:
            if heading:
                print(f"  {heading}")
        sys.exit(1)

    heading, block_text = blocks[idx]
    print(f"완료 처리: {heading}")

    # status를 done으로 변경
    updated_block = set_status(block_text, "done")

    # resolution 추가
    resolution_text = args.resolution if args.resolution else "처리"
    updated_block = append_resolution(updated_block, resolution_text)

    # done 파일에 추가
    append_to_done(DONE_FILE, updated_block)
    print(f"  → {DONE_FILE.relative_to(REPO_ROOT)} 에 추가됨")

    # 큐에서 제거
    remaining_blocks = [body for i, (_, body) in enumerate(blocks) if i != idx]
    new_items_text = "".join(remaining_blocks)
    new_queue = pre_section + new_items_text + tail_text
    QUEUE_FILE.write_text(new_queue, encoding="utf-8")
    print(f"  → {QUEUE_FILE.relative_to(REPO_ROOT)} 에서 제거됨")


if __name__ == "__main__":
    main()
