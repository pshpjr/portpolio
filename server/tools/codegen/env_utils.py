"""Minimal .env loader for codegen scripts.

Priority (highest → lowest):
  1. Environment variables already set (CI/CD pipeline vars)
  2. .env file in the repo root (local dev)
  3. Script-level defaults

Usage:
    from env_utils import load_dotenv, REPO_ROOT
    load_dotenv(REPO_ROOT / ".env")
"""

from __future__ import annotations

import os
from pathlib import Path

SCRIPT_DIR = Path(__file__).resolve().parent
REPO_ROOT = SCRIPT_DIR.parents[2]


def load_dotenv(path: Path) -> None:
    """Load KEY=VALUE pairs from *path* into os.environ.

    Existing environment variables are never overwritten, so CI/CD vars
    always take precedence over the .env file.
    """
    if not path.exists():
        return
    for raw in path.read_text(encoding="utf-8").splitlines():
        line = raw.strip()
        if not line or line.startswith("#") or "=" not in line:
            continue
        key, _, value = line.partition("=")
        key = key.strip()
        value = value.strip().strip('"').strip("'")
        os.environ.setdefault(key, value)
