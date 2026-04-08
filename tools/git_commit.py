#!/usr/bin/env python3
"""
git_commit.py — Claude Code에서 git commit을 안전하게 실행하는 래퍼.

Bash 툴은 Windows 경로(D:/...)로 실행되므로 /mnt/ 경로 오류가 발생한다.
이 스크립트는 저장소 루트를 자동 감지해 올바른 경로로 git을 실행한다.

사용법:
  python tools/git_commit.py -m "커밋 메시지"
  python tools/git_commit.py --add <파일1> <파일2> -m "커밋 메시지"
  python tools/git_commit.py --add-all -m "커밋 메시지"
  python tools/git_commit.py --status
  python tools/git_commit.py --diff
"""

import argparse
import subprocess
import sys
from pathlib import Path


def find_repo_root() -> Path:
    """현재 파일 위치에서 git 루트를 찾는다."""
    candidate = Path(__file__).resolve().parent
    while candidate != candidate.parent:
        if (candidate / ".git").exists():
            return candidate
        candidate = candidate.parent
    raise RuntimeError("git 저장소 루트를 찾을 수 없습니다.")


def run(args: list[str], cwd: Path) -> int:
    result = subprocess.run(args, cwd=str(cwd))
    return result.returncode


def main() -> None:
    parser = argparse.ArgumentParser(description="git 커밋 래퍼 (Windows 경로 오류 방지)")
    parser.add_argument("-m", "--message", help="커밋 메시지")
    parser.add_argument("--add", nargs="*", metavar="FILE", help="스테이징할 파일 목록")
    parser.add_argument("--add-all", action="store_true", help="변경된 추적 파일 전체 스테이징 (git add -u)")
    parser.add_argument("--status", action="store_true", help="git status 출력")
    parser.add_argument("--diff", action="store_true", help="git diff (staged) 출력")
    parser.add_argument("--log", action="store_true", help="최근 커밋 5개 출력")
    opts = parser.parse_args()

    root = find_repo_root()

    if opts.status:
        run(["git", "status"], root)
        return

    if opts.diff:
        run(["git", "diff", "--staged"], root)
        return

    if opts.log:
        run(["git", "log", "--oneline", "-5"], root)
        return

    if not opts.message:
        parser.error("-m / --message 는 필수입니다.")

    # 스테이징
    if opts.add_all:
        code = run(["git", "add", "-u"], root)
        if code != 0:
            sys.exit(code)
    elif opts.add:
        code = run(["git", "add"] + opts.add, root)
        if code != 0:
            sys.exit(code)

    # 커밋
    message = opts.message + "\n\nCo-Authored-By: Claude Sonnet 4.6 <noreply@anthropic.com>"
    code = run(["git", "commit", "-m", message], root)
    sys.exit(code)


if __name__ == "__main__":
    main()
