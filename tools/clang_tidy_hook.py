"""PostToolUse hook: run clang-format + clang-tidy --fix on server/lib .h/.cpp/.hpp files."""

import json
import os
import subprocess
import sys

_LLVM_BIN = (
    r"C:\Program Files\Microsoft Visual Studio\2022\Professional"
    r"\VC\Tools\Llvm\x64\bin"
)

CLANG_FORMAT = os.path.join(_LLVM_BIN, "clang-format.exe")
CLANG_TIDY = os.path.join(_LLVM_BIN, "clang-tidy.exe")

REPO_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
LIB_DIR = os.path.join(REPO_ROOT, "server", "lib")

EXTENSIONS = {".h", ".hpp", ".cpp"}


def main() -> None:
    data = json.loads(sys.stdin.read())
    tool_input = data.get("tool_input", {})
    tool_response = data.get("tool_response", {})

    file_path = tool_input.get("file_path") or tool_response.get("filePath", "")
    if not file_path:
        return

    file_path = os.path.abspath(file_path)

    _, ext = os.path.splitext(file_path)
    if ext not in EXTENSIONS:
        return

    if not file_path.startswith(LIB_DIR):
        return

    if os.path.isfile(CLANG_FORMAT):
        subprocess.run(
            [CLANG_FORMAT, "-i", file_path],
            cwd=LIB_DIR,
            timeout=15,
            capture_output=True,
        )

    if os.path.isfile(CLANG_TIDY):
        subprocess.run(
            [CLANG_TIDY, "--fix", "--fix-errors", file_path],
            cwd=LIB_DIR,
            timeout=30,
            capture_output=True,
        )


if __name__ == "__main__":
    main()
