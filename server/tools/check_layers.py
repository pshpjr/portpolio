#!/usr/bin/env python3
"""Backward-compatible wrapper for the shared layer checker."""

from __future__ import annotations

import runpy
from pathlib import Path


if __name__ == "__main__":
    runpy.run_path(
        str(Path(__file__).resolve().parents[2] / "tools" / "check_layers.py"),
        run_name="__main__",
    )
