# Combat Formula Draft

## 목표
전투 스탯 초안을 바탕으로 기획 단계에서 필요한 핵심 전투 공식 초안을 문서화하고, 후속 밸런스 엑셀과 구현 문서가 같은 계산 방향을 공유하도록 한다.

## 범위
- `docs/proposal/ingame/combat_formulas.md`
- `docs/proposal/ingame/combat.md`
- `docs/proposal/ingame/combat_stats.md`
- `docs/proposal/main.md`

## 완료 기준
- [x] 전투 공식 초안 문서가 추가된다.
- [x] 데미지, 속성, 무력화, 상태 이상, 위협도, 자원/쿨다운의 기본 계산식이 포함된다.
- [x] 미확정 항목과 조정 포인트가 명시된다.
- [x] 허브와 관련 문서에서 새 문서로 진입할 수 있다.

## 진행 상황
- [x] 기존 전투/스탯 문서를 다시 검토했다.
- [x] 공식 묶음과 공통 표기 규칙을 확정한다.
- [x] 공식 초안 문서와 관련 링크를 추가한다.
- [x] 문서 검증을 실행한다.

## 설계 메모
- 현재 단계의 목적은 밸런스 수치 확정이 아니라 계산 구조를 정하는 것이다.
- 방어/저항은 과도한 무한 스케일링을 막기 위해 분모형 감쇠 공식을 우선 사용한다.
- 무력화와 상태 이상은 누적 저항이 있는 보스전 구조를 전제로 한다.
- `python tools/doc_check.py`는 실행했지만, 이번 변경과 무관한 `.vcpkg-downloads/.../README.md -> CODE_OF_CONDUCT.md` 깨진 링크 1건 때문에 전체 실패로 보고되었다.
