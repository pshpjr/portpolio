#!/usr/bin/env python3
"""Write a structured post-response agent note and optionally append a queue item."""

from __future__ import annotations

import argparse
from datetime import datetime
from pathlib import Path
import re
import sys


REPO_ROOT = Path(__file__).resolve().parents[1]
QUEUE_PATH = REPO_ROOT / "docs" / "workflow" / "harness-improvement-queue.md"
NOTES_DIR = REPO_ROOT / "_workspace" / "agent-notes"


def slugify(text: str) -> str:
    text = text.strip().lower()
    text = re.sub(r"[^a-z0-9]+", "-", text)
    return text.strip("-") or "note"


def append_queue_item(
    *,
    task: str,
    message: str,
    suggested_agent: str,
    area: str,
    recommended_artifact: str,
    context_savings: str,
    token_meter: str,
    source: str,
) -> None:
    if not QUEUE_PATH.exists():
        raise FileNotFoundError(f"missing queue file: {QUEUE_PATH}")

    content = QUEUE_PATH.read_text(encoding="utf-8")
    marker = "## Claimed"
    if marker not in content:
        raise RuntimeError("queue file is missing the '## Claimed' section")

    today = datetime.now().strftime("%Y-%m-%d")
    title = slugify(task)[:60]
    item = (
        f"## {today} - {title}\n"
        f"- status: open\n"
        f"- suggested-agent: {suggested_agent}\n"
        f"- area: {area}\n"
        f"- recommended-artifact: {recommended_artifact}\n"
        f"- context-savings: {context_savings}\n"
        f"- token-meter: {token_meter}\n"
        f"- task: {task}\n"
        f"- message: {message}\n"
        f"- source: {source}\n\n"
    )
    QUEUE_PATH.write_text(content.replace(marker, item + marker), encoding="utf-8")


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Record a structured post-response agent note and optionally append a queue item."
    )
    parser.add_argument("--agent", required=True, help="Agent or tool name.")
    parser.add_argument("--summary", required=True, help="Short response summary.")
    parser.add_argument("--source", required=True, help="Task, command, or file that produced the response.")
    parser.add_argument("--token-note", default="", help="Optional token meter summary.")
    parser.add_argument("--decision", action="append", default=[], help="Decision or conclusion line. Can be repeated.")
    parser.add_argument("--task", default="", help="Optional follow-up task to append to the improvement queue.")
    parser.add_argument("--message", default="", help="Optional follow-up context for the queue item.")
    parser.add_argument("--suggested-agent", default="harness-improver")
    parser.add_argument("--area", default="workflow")
    parser.add_argument("--recommended-artifact", default="other")
    parser.add_argument("--context-savings", default="unknown")
    parser.add_argument("--queue-token-meter", default="")
    args = parser.parse_args()

    NOTES_DIR.mkdir(parents=True, exist_ok=True)
    timestamp = datetime.now().strftime("%Y%m%d-%H%M%S")
    note_path = NOTES_DIR / f"{timestamp}-{slugify(args.agent)}.md"

    note_lines = [
        f"# Agent Response Artifact - {args.agent}",
        "",
        f"- created-at: {timestamp}",
        f"- source: {args.source}",
        f"- summary: {args.summary}",
    ]
    if args.token_note:
        note_lines.append(f"- token-note: {args.token_note}")
    note_lines.append("")

    if args.decision:
        note_lines.append("## Decisions")
        note_lines.append("")
        for decision in args.decision:
            note_lines.append(f"- {decision}")
        note_lines.append("")

    if args.task or args.message:
        if not (args.task and args.message):
            raise ValueError("--task and --message must be provided together")
        note_lines.extend(
            [
                "## Queued Follow-Up",
                "",
                f"- task: {args.task}",
                f"- message: {args.message}",
                f"- suggested-agent: {args.suggested_agent}",
                f"- area: {args.area}",
                f"- recommended-artifact: {args.recommended_artifact}",
                f"- context-savings: {args.context_savings}",
                f"- token-meter: {args.queue_token_meter or args.token_note or 'pending'}",
                "",
            ]
        )
        append_queue_item(
            task=args.task,
            message=args.message,
            suggested_agent=args.suggested_agent,
            area=args.area,
            recommended_artifact=args.recommended_artifact,
            context_savings=args.context_savings,
            token_meter=args.queue_token_meter or args.token_note or "pending",
            source=args.source,
        )

    note_path.write_text("\n".join(note_lines).rstrip() + "\n", encoding="utf-8")
    print(note_path.relative_to(REPO_ROOT))
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except Exception as exc:  # pragma: no cover - CLI failure path
        print(f"ERROR: {exc}", file=sys.stderr)
        raise SystemExit(1)
