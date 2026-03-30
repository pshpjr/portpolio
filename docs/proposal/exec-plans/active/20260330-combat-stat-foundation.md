# Combat Stat Foundation

## 목표
기존 전투 기획 문서를 바탕으로 전투 스탯 목록과 핵심 전투 공식 문서를 추가해, 이후 밸런스 수치와 엑셀 데이터를 같은 구조 위에서 정리할 수 있게 한다.

## 범위
- `docs/proposal/ingame/combat_stats.md`
- `docs/proposal/ingame/combat_formulas.md`
- `docs/proposal/ingame/combat.md`
- `docs/proposal/main.md`
- `docs/proposal/exec-plans/active/INDEX.md`

## 완료 기준
- [x] 전투 스탯 문서가 추가된다.
- [x] 전투 공식 문서가 추가된다.
- [x] 관련 허브 문서와 링크가 연결된다.
- [x] 문서 검증 결과를 기록한다.
- [ ] 변경분만 선별 커밋한다.

## 진행 상황
- [x] 현재 proposal 문서와 active plan 인덱스를 확인했다.
- [x] 전투 스탯/공식 문서를 추가한다.
- [x] 링크와 허브 문서를 갱신한다.
- [ ] 문서 검증과 커밋을 수행한다.

## 설계 메모
- 수치값 자체보다 계산 구조를 먼저 고정한다.
- 무기 기반 정체성, 패링, 무력화, 속성 상호작용, 위협도 운용을 모두 표현할 수 있어야 한다.
- 방어, 저항, 무력화 저항은 감쇠 구조를 우선 사용해 값 폭주를 피한다.
- `python tools/doc_check.py`는 실행했지만, 이번 변경과 무관한 `.vcpkg-downloads/.../README.md -> CODE_OF_CONDUCT.md` 깨진 링크 1건 때문에 전체 실패로 보고되었다.
