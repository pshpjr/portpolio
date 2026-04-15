#!/usr/bin/env python3
"""Server build entrypoint for shared packet code generation."""

from __future__ import annotations

import argparse
import os
import subprocess
import sys
from pathlib import Path

from env_utils import REPO_ROOT, load_dotenv

load_dotenv(REPO_ROOT / ".env")

SCRIPT_DIR = Path(__file__).resolve().parent
DEFAULT_SCHEMA_DIR = REPO_ROOT / "shared" / "protocol"
DEFAULT_SERVER_OUT = Path(os.environ.get("PORTPOLIO_SERVER_OUT", str(REPO_ROOT / "server" / "src" / "generated")))
DEFAULT_UNREAL_OUT = Path(os.environ.get("PORTPOLIO_UNREAL_OUT", str(REPO_ROOT / "client" / "Source" / "client" / "Public" / "Generated")))
DEFAULT_PROTOC     = os.environ.get("PROTOC_PATH")
DEFAULT_BUILDER = DEFAULT_SCHEMA_DIR / "build-proto.py"


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Generate shared packet code for server and Unreal")
    parser.add_argument("--schema-dir", type=Path, default=DEFAULT_SCHEMA_DIR)
    parser.add_argument("--server-out", type=Path, default=DEFAULT_SERVER_OUT)
    parser.add_argument("--unreal-out", type=Path, default=DEFAULT_UNREAL_OUT)
    parser.add_argument("--protoc", type=Path, default=Path(DEFAULT_PROTOC) if DEFAULT_PROTOC else None)
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
