#!/usr/bin/env python3
"""Placeholder FlatBuffers code generator entrypoint."""

import argparse


def main() -> int:
    parser = argparse.ArgumentParser(description="FlatBuffers wrapper generator")
    parser.add_argument("--schema-dir", default="schemas")
    parser.parse_args()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
