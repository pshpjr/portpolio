#!/usr/bin/env python3
"""Backward-compatible wrapper for the shared documentation checker."""

from __future__ import annotations

import runpy
from pathlib import Path


if __name__ == "__main__":
    runpy.run_path(
        str(Path(__file__).resolve().parents[2] / "tools" / "doc_check.py"),
        run_name="__main__",
    )
