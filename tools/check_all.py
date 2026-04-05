#!/usr/bin/env python3
"""Run all standard repository checks in one pass.

Equivalent to running manually:
    python tools/doc_check.py
    python tools/check_encoding.py
    python tools/check_layers.py --root server/src --layer-order types,config,core,service,network,runtime
    python tools/check_layers.py --root Lib --layer-order include,src,tests

Usage:
    python tools/check_all.py            # all checks
    python tools/check_all.py --only docs encoding   # subset
    python tools/check_all.py --skip layers          # skip layer checks
"""

from __future__ import annotations

import argparse
import subprocess
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[1]

CHECKS: list[dict] = [
    {
        "id": "docs",
        "label": "doc_check (links + exec-plan format)",
        "cmd": [sys.executable, "tools/doc_check.py"],
    },
    {
        "id": "encoding",
        "label": "check_encoding (UTF-8 without BOM)",
        "cmd": [sys.executable, "tools/check_encoding.py"],
    },
    {
        "id": "layers-server",
        "label": "check_layers server/src",
        "cmd": [
            sys.executable,
            "tools/check_layers.py",
            "--root", "server/src",
            "--layer-order", "types,config,core,service,network,runtime",
        ],
        "optional": True,  # skip silently if directory does not exist
    },
    {
        "id": "layers-lib",
        "label": "check_layers Lib",
        "cmd": [
            sys.executable,
            "tools/check_layers.py",
            "--root", "Lib",
            "--layer-order", "include,src,tests",
        ],
        "optional": True,
    },
]


def run_check(check: dict, verbose: bool) -> bool:
    """Return True if check passed (or was skipped as optional-missing)."""
    label = check["label"]
    cmd = check["cmd"]
    optional = check.get("optional", False)

    # For optional checks, verify the target directory exists first.
    if optional:
        # Extract --root argument value to test existence.
        try:
            root_idx = cmd.index("--root") + 1
            root_path = REPO_ROOT / cmd[root_idx]
            if not root_path.exists():
                print(f"  SKIP  {label} (directory not found)")
                return True
        except (ValueError, IndexError):
            pass

    result = subprocess.run(
        cmd,
        cwd=REPO_ROOT,
        capture_output=not verbose,
        text=True,
    )

    if result.returncode == 0:
        print(f"  PASS  {label}")
        return True
    else:
        print(f"  FAIL  {label}")
        if not verbose and result.stdout:
            # Always show stdout on failure even without --verbose.
            for line in result.stdout.splitlines():
                print(f"        {line}")
        if not verbose and result.stderr:
            for line in result.stderr.splitlines():
                print(f"        {line}")
        return False


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Run all standard repository checks in one pass."
    )
    parser.add_argument(
        "--only",
        nargs="*",
        metavar="ID",
        help="Run only these check IDs (docs, encoding, layers-server, layers-lib).",
    )
    parser.add_argument(
        "--skip",
        nargs="*",
        metavar="ID",
        default=[],
        help="Skip these check IDs.",
    )
    parser.add_argument(
        "--verbose",
        "-v",
        action="store_true",
        help="Stream check output directly (default: show only on failure).",
    )
    args = parser.parse_args()

    selected = CHECKS
    if args.only is not None:
        ids = set(args.only)
        selected = [c for c in CHECKS if c["id"] in ids]
    if args.skip:
        skip_ids = set(args.skip)
        selected = [c for c in selected if c["id"] not in skip_ids]

    if not selected:
        print("No checks selected.")
        return 0

    print(f"Running {len(selected)} check(s)...")
    failures: list[str] = []
    for check in selected:
        passed = run_check(check, verbose=args.verbose)
        if not passed:
            failures.append(check["label"])

    print()
    if failures:
        print(f"FAILED ({len(failures)}/{len(selected)}):")
        for f in failures:
            print(f"  - {f}")
        return 1

    print(f"All {len(selected)} check(s) passed.")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except Exception as exc:  # pragma: no cover
        print(f"ERROR: {exc}", file=sys.stderr)
        raise SystemExit(1)
