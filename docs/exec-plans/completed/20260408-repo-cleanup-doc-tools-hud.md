# Exec Plan: 저장소 정리 - 중복 문서, 미사용 tools, client HUD 문서 이동

> 생성: 2026-04-08
> 완료: 2026-04-08
> 영역: `docs/`, `client/docs/`, `tools/`, `docs/exec-plans/`

## 목표

중복된 루트 C++ 스타일 문서를 제거하고, 참조되지 않는 tools 스크립트를 삭제하며, client HUD/Widget 가이드라인을 client 문서 영역으로 이동한다.

## 범위

- 루트 C++ 스타일 문서 참조를 서버 문서 경로로 갱신한 뒤 중복 파일 삭제
- 지정된 generator/review tools 스크립트 3개 참조 확인 후 미사용이면 삭제
- HUD/Widget 가이드라인을 client 문서 영역으로 이동하고 프로젝트 특화 규칙 중심으로 폴리싱
- 이동/삭제에 따른 Markdown 링크 갱신 및 문서 검사 실행

## 단계

- [x] 1. 관련 문서와 스크립트 참조 검색
- [x] 2. 링크 경로 갱신 및 중복/미사용 파일 삭제
- [x] 3. client HUD/Widget 가이드라인 이동 및 내용 정리
- [x] 4. 문서 검사와 git diff 확인
- [x] 5. 완료 상태 반영 후 커밋 준비

## 완료 기준

- [x] 루트 C++ 스타일 문서가 제거되고 남은 참조는 `server/docs/conventions/cpp-style.md`를 가리킨다.
- [x] 지정된 tools 스크립트 3개가 필수 문서/스크립트에서 참조되지 않는 경우 삭제된다.
- [x] client HUD/Widget 문서가 `client/docs/` 아래에 있으며 프로젝트 특화 규칙만 담는다.
- [x] `python tools/doc_check.py`가 실행되었고 결과가 확인된다.
- [x] 요청된 메시지로 git commit 준비가 완료된다.
