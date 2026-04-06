# Exec Plan: 언리얼 인벤토리 구현 완성

> 생성: 2026-04-05
> 영역: `client/`

## 목적

`client-inventory-foundation`에서 골격만 잡힌 인벤토리 시스템을 기획 명세와 일치하게 완성한다.
에디터 내부(Blueprint) 작업은 코드로 대체할 수 없으므로, 사용자가 해야 할 에디터 작업을 문서로 정리한다.

## 현재 상태 (2026-04-05 기준)

**완료된 C++ 코드:**
- `ClientInventoryTypes.h` — 모든 타입/enum/struct
- `ClientItemInstance` — 스택·내구도·거래횟수·위치 관리
- `ClientInventoryComponent` — 슬롯 add/remove/move/stack
- `ClientEquipmentComponent` — equip/unequip per slot
- `ClientSkillHotbarComponent` — 스킬 핫바 등록/해제
- `ClientStorageComponent` — 탭 기반 공유창고
- Widget base classes (4종) — bind + delegate + BlueprintImplementableEvent

**남은 C++ 작업:**
1. 장비 슬롯 enum 기획 불일치 수정 (Head, Feet 제거)
2. `EquipItem` 교체(swap) 장착 지원
3. 소모품 퀵슬롯 컴포넌트 추가
4. 소모품 퀵슬롯 위젯 베이스 추가

**에디터(사용자) 작업:**
- `WBP_InventoryHUD` Blueprint 구현
- `WBP_EquipmentPanel` 신규 생성
- `WBP_ConsumableQuickSlot` 신규 생성
- `BP_TopDownController` 컴포넌트 추가 + 위젯 스폰/바인딩

## 단계

- [x] 1. 장비 슬롯 enum 기획 불일치 수정
- [x] 2. EquipItem 교체(swap) 장착 지원
- [x] 3. 소모품 퀵슬롯 컴포넌트 + 위젯 베이스 추가
- [x] 4. 에디터 작업 가이드 문서 작성
- [ ] 5. 빌드 검증 — 사용자가 직접 수행 시도

## 완료 기준

- [ ] `EClientEquipSlot`이 기획서 슬롯 7개(Weapon, Chest, Legs, Hands, Necklace, RingLeft, RingRight)와 일치한다.
- [ ] 이미 장착된 슬롯에 새 아이템을 장착하면 기존 아이템이 반환된다.
- [ ] 소모품 퀵슬롯 2슬롯, 각 5개 스택 컴포넌트가 존재한다.
- [ ] 에디터 작업 가이드가 `docs/` 아래에 생성된다.
- [ ] 빌드가 통과하거나 오류 원인이 기록된다.
