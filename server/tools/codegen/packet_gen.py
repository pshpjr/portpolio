#!/usr/bin/env python3
"""Server build entrypoint for shared packet code generation."""

from __future__ import annotations

import argparse
import subprocess
import sys
from pathlib import Path


SCRIPT_DIR = Path(__file__).resolve().parent
REPO_ROOT = SCRIPT_DIR.parents[2]
DEFAULT_SCHEMA_DIR = REPO_ROOT / "shared" / "protocol"
DEFAULT_SERVER_OUT = REPO_ROOT / "shared" / "generated" / "server"
DEFAULT_UNREAL_OUT = REPO_ROOT / "shared" / "generated" / "unreal"
DEFAULT_BUILDER = DEFAULT_SCHEMA_DIR / "build-proto.py"


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Generate shared packet code for server and Unreal")
    parser.add_argument("--schema-dir", type=Path, default=DEFAULT_SCHEMA_DIR)
    parser.add_argument("--server-out", type=Path, default=DEFAULT_SERVER_OUT)
    parser.add_argument("--unreal-out", type=Path, default=DEFAULT_UNREAL_OUT)
    parser.add_argument("--protoc", type=Path, default=None)
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    builder = DEFAULT_BUILDER.resolve()
    if not builder.exists():
        raise FileNotFoundError(f"Shared protocol builder not found: {builder}")

    command = [
        sys.executable,
        str(builder),
        "--schema-dir",
        str(args.schema_dir.resolve()),
        "--server-out",
        str(args.server_out.resolve()),
        "--unreal-out",
        str(args.unreal_out.resolve()),
    ]

    if args.protoc is not None:
        command.extend(["--protoc", str(args.protoc.resolve())])

    completed = subprocess.run(command, cwd=REPO_ROOT)
    return completed.returncode


if __name__ == "__main__":
    raise SystemExit(main())
