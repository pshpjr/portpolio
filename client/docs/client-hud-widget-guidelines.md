# Client HUD / Widget Guidelines

## 목적

`client/` UI는 전투 HUD, 인벤토리/장비/창고, 파티 상태, 업적 토스트처럼 성격이 다른 화면을 함께 다룬다. 이 문서는 언리얼 일반론이 아니라 현재 프로젝트의 HUD/Widget 책임 경계를 정한다.

---

## 기본 구조

- `ClientHUD` 또는 `ClientPlayerController` 중 하나를 UI 루트 오케스트레이터로 정한다.
- 상시 HUD, 메뉴형 화면, 팝업/토스트는 같은 레벨에 나열하지 말고 성격별 컨테이너로 묶는다.
- 기능 위젯은 화면 컨테이너 아래에서 재사용 가능하게 만든다.

```text
Client UI Root
├─ CombatHudRoot
│  ├─ StatusBarsWidget
│  ├─ ClientSkillHotbarWidgetBase
│  ├─ ClientConsumableQuickSlotWidgetBase
│  └─ ClientPartyStatusWidget
├─ MenuScreenRoot
│  ├─ InventoryScreenWidget
│  ├─ StorageScreenWidget
│  ├─ CharacterMenuWidget
│  └─ SkillTreeScreenWidget
└─ NotificationRoot
   ├─ ClientAchievementToastWidget
   └─ ClientConsentPromptWidget
```

---

## 현재 위젯 베이스 책임

| 위젯 | 책임 |
|---|---|
| `ClientSkillHotbarWidgetBase` | 전투 스킬 슬롯 표시, 쿨다운/강조/입력 힌트 표현 |
| `ClientConsumableQuickSlotWidgetBase` | 소비 아이템 퀵슬롯 표시와 수량/사용 가능 상태 갱신 |
| `ClientInventoryWidgetBase` | 인벤토리 슬롯 목록, 선택 상태, 드래그 앤 드롭 표현 |
| `ClientEquipmentWidgetBase` | 장비 슬롯과 장착 상태 표시 |
| `ClientStorageWidgetBase` | 창고 슬롯과 캐릭터 인벤토리 간 이동 UI 표현 |
| `ClientPartyStatusWidget` | 1~4인 파티원의 HP, 접속, 리더/로컬 플레이어 상태 표시 |
| `ClientAchievementToastWidget` | 업적 달성 알림 큐와 현재 토스트 상태 표시 |
| `ClientConsentPromptWidget` | 파티 초대, 던전 입장, 매칭 준비, 거래 요청 같은 동의 팝업 표시 |

위젯은 상태를 소유하지 않고 표현과 사용자 상호작용만 담당한다. 인벤토리/장비/창고/핫바/퀵슬롯 상태는 대응 컴포넌트가 단일 소스다.

---

## 화면 구성 규칙

- 전투 중 항상 보여야 하는 것은 `CombatHudRoot` 아래에 둔다.
- 인벤토리와 장비는 사용 흐름이 강하게 연결되므로 `InventoryScreenWidget` 아래 패널로 함께 배치한다.
- 창고는 NPC/마을 상호작용 화면으로 보고 `StorageScreenWidget`에서 `ClientStorageWidgetBase`와 `ClientInventoryWidgetBase`를 함께 사용한다.
- 캐릭터 정보와 스킬트리는 전투 준비/성장 화면군으로 분리하고, 필요하면 메뉴 탭이나 별도 화면 컨테이너로 묶는다.
- 업적과 동의 요청은 HUD 루트에 상시 패널로 두지 말고 `NotificationRoot`에서 토스트/팝업으로 처리한다.

---

## 바인딩 규칙

현재 베이스 위젯은 컴포넌트를 명시적으로 바인딩받는 구조를 따른다.

- `BindInventoryComponent()`
- `BindEquipmentComponent()`
- `BindStorageComponent()`
- `BindSkillHotbarComponent()`
- `BindConsumableQuickSlotComponent()`

권장 흐름:

1. UI 루트가 위젯을 생성한다.
2. UI 루트가 로컬 플레이어의 관련 컴포넌트를 찾는다.
3. UI 루트가 각 위젯에 컴포넌트를 바인딩한다.
4. 위젯은 컴포넌트 delegate를 구독한다.
5. 상태 변경 시 `RefreshFrom...()` 또는 `OnInventoryUpdated()` 블루프린트 이벤트로 화면만 갱신한다.

위젯이 직접 플레이어를 탐색해 필요한 컴포넌트를 찾는 방식은 피한다. 의존성을 루트에서 주입해야 재생성, 테스트 더블, 로컬 플레이어 교체에 대응하기 쉽다.

---

## 입력과 도메인 슬롯 분리

스킬 슬롯과 퀵슬롯 enum은 `Q`, `E`, `R`, `F` 같은 키 이름이 아니라 의미 기반 슬롯명으로 둔다.

- 예: `Active1`, `Active2`, `Active3`, `Active4`, `Identity`
- 입력 매핑은 키보드, 게임패드, 리바인딩 계층에서 별도로 연결한다.
- 저장 데이터와 서버 계약에는 입력 디바이스 이름을 넣지 않는다.

---

## 갱신 규칙

- 상태 변경은 컴포넌트 delegate 기반 갱신을 우선한다.
- 위젯 `Tick`은 드래그 중 하이라이트, 쿨다운 애니메이션 보간, 툴팁 위치 보정처럼 프레임 의존 표현에만 제한한다.
- 장착 가능 여부, 스택 병합, 슬롯 이동 같은 최종 게임 규칙은 위젯이 아니라 컴포넌트나 도메인 계층에서 판단한다.
- UI는 이미 계산된 값과 표시 상태를 받아 렌더링한다.

---

## 피해야 할 구조

- 모든 패널을 UI 루트 직속 최상위 위젯으로 나열하는 구조
- 위젯이 게임 규칙과 서버 패킷 조립을 직접 소유하는 구조
- 위젯이 로컬 플레이어를 직접 탐색해 컴포넌트를 가져오는 구조
- 입력키 이름을 도메인 슬롯명이나 저장 데이터로 사용하는 구조
