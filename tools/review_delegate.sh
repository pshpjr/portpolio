#!/usr/bin/env bash
# review_delegate.sh — Codex/OpenCode 리뷰 위임 래퍼
# 용도: 외부 LLM에 리뷰를 위임하고 정제된 결과만 반환
#
# 사용법:
#   tools/review_delegate.sh <engine> <prompt>
#   tools/review_delegate.sh codex  "리뷰 프롬프트..."
#   tools/review_delegate.sh opencode "리뷰 프롬프트..."
#
# 출력: stdout에 최종 리뷰 텍스트만 출력 (중간 로그 제거)
# 종료코드: 0=성공, 1=엔진 오류, 2=사용법 오류

set -euo pipefail

ENGINE="${1:-}"
shift || true
PROMPT="$*"

if [[ -z "$ENGINE" || -z "$PROMPT" ]]; then
  echo "usage: review_delegate.sh <codex|opencode> <prompt>" >&2
  exit 2
fi

TMPDIR="${TMPDIR:-/tmp}"
RESULT_FILE="$TMPDIR/review_delegate_$(date +%s).txt"

case "$ENGINE" in
  codex)
    codex exec \
      --sandbox read-only \
      --ephemeral \
      -o "$RESULT_FILE" \
      "$PROMPT" > /dev/null 2>&1

    if [[ -f "$RESULT_FILE" ]]; then
      cat "$RESULT_FILE"
      rm -f "$RESULT_FILE"
    else
      echo "ERROR: codex produced no output" >&2
      exit 1
    fi
    ;;

  opencode)
    # opencode --format json 출력에서 마지막 text 이벤트의 text 필드 추출
    RAW_FILE="$TMPDIR/review_delegate_raw_$(date +%s).jsonl"

    opencode run \
      --format json \
      --agent portpolio-review \
      "$PROMPT" > "$RAW_FILE" 2>/dev/null

    # 마지막 type=text 이벤트에서 .part.text 추출
    if command -v jq &> /dev/null; then
      grep '"type":"text"' "$RAW_FILE" \
        | tail -1 \
        | jq -r '.part.text' > "$RESULT_FILE"
    else
      # jq 없으면 python fallback
      python -c "
import json, sys
last_text = ''
for line in open(sys.argv[1], encoding='utf-8'):
    line = line.strip()
    if not line:
        continue
    try:
        obj = json.loads(line)
        if obj.get('type') == 'text':
            last_text = obj.get('part', {}).get('text', '')
    except json.JSONDecodeError:
        pass
print(last_text)
" "$RAW_FILE" > "$RESULT_FILE"
    fi

    rm -f "$RAW_FILE"

    if [[ -s "$RESULT_FILE" ]]; then
      cat "$RESULT_FILE"
      rm -f "$RESULT_FILE"
    else
      echo "ERROR: opencode produced no text output" >&2
      exit 1
    fi
    ;;

  *)
    echo "ERROR: unknown engine '$ENGINE' (use codex or opencode)" >&2
    exit 2
    ;;
esac
