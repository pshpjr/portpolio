# 인벤토리 에디터 작업 가이드

> 이 문서는 C++ 코드만으로 완성할 수 없는 언리얼 에디터 내 Blueprint/Widget 작업을 단계별로 설명합니다.

---

## 전제 조건

- 빌드가 통과된 상태여야 합니다 (`Build.bat clientEditor Win64 Development`).
- 에디터를 재시작해 새 C++ 클래스(컴포넌트, 위젯 베이스)가 반영되어 있어야 합니다.

---

## 1단계: BP_TopDownController — 컴포넌트 추가

`Content/TopDown/Blueprints/BP_TopDownController`를 엽니다.

### 추가할 컴포넌트 (Components 패널)

| 컴포넌트 클래스 | 변수명 | 비고 |
|---|---|---|
| `ClientInventoryComponent` | `InventoryComponent` | SlotCount = 32 |
| `ClientEquipmentComponent` | `EquipmentComponent` | 기본값 사용 |
| `ClientSkillHotbarComponent` | `SkillHotbarComponent` | 기본값 사용 |
| `ClientStorageComponent` | `StorageComponent` | 공유 창고 |
| `ClientConsumableQuickSlotComponent` | `ConsumableQuickSlotComponent` | 기본값 사용 |

### BeginPlay 로직 (Event Graph)

```
Event BeginPlay
  → InventoryComponent → InitializeInventory(32)
  → EquipmentComponent → InitializeDefaultSlots()
  → StorageComponent → InitializeDefaultSharedStorage(TabCount=1, SlotsPerTab=48)
  → [위젯 스폰 및 바인딩 — 2단계 참조]
```

---

## 2단계: WBP_InventoryHUD 구현

`Content/TopDown/UI/WBP_InventoryHUD`를 엽니다.
이미 존재하는 파일입니다. 이 위젯의 Parent Class를 **`ClientInventoryWidgetBase`**로 변경합니다.

> Widget Blueprint → Graph → Class Settings → Parent Class → `ClientInventoryWidgetBase`

### Designer 탭 — 필요한 위젯 구성

```
WBP_InventoryHUD
└── CanvasPanel (루트)
    └── Border (배경 패널)
        └── VerticalBox
            ├── TextBlock (제목: "인벤토리")
            └── ScrollBox (ItemList)
                └── [아이템 위젯 동적 생성 — 아래 참조]
```

**아이템 엔트리 위젯 (WBP_InventoryItem 으로 분리 권장):**
```
Button
└── HorizontalBox
    ├── Image (ItemIcon)
    ├── TextBlock (ItemName)
    └── TextBlock (StackCount)  — MaxStackCount > 1일 때만 표시
```

### Graph 탭 — OnInventoryUpdated 구현

서버는 슬롯 개념을 사용하지 않습니다. 아이템은 항상 자동 정렬됩니다.

```
Event OnInventoryUpdated(ChangedItem)
  → Branch: ChangedItem == null?
      → True  (전체 재갱신 필요 — 제거·초기화):
              ItemList 자식 위젯 전부 Clear
              GetInventoryComponent → GetItems → ForEachLoop
                  → Create Widget(WBP_InventoryItem) → 아이템 데이터 바인딩 → Add to ItemList
      → False (단건 추가·수량 변경):
              ItemList에서 ChangedItem의 ItemKey를 가진 위젯 탐색
              → 있으면: 수량 텍스트만 업데이트
              → 없으면: Create Widget(WBP_InventoryItem) → SortItems 순서에 맞게 삽입
```

> **단순화 팁:** 아이템 수가 적은 포트폴리오 규모라면 ChangedItem 유무와 관계없이 항상 전체 재갱신해도 됩니다.

---

## 3단계: WBP_EquipmentPanel 신규 생성

`Content/TopDown/UI/` 에 새 Widget Blueprint를 생성합니다.
- 이름: `WBP_EquipmentPanel`
- Parent Class: `ClientEquipmentWidgetBase`

### Designer 탭 구성

```
WBP_EquipmentPanel
└── CanvasPanel
    └── GridPanel (또는 Canvas로 직접 배치)
        ├── WBP_EquipSlot (Weapon)
        ├── WBP_EquipSlot (Chest)
        ├── WBP_EquipSlot (Legs)
        ├── WBP_EquipSlot (Hands)
        ├── WBP_EquipSlot (Necklace)
        ├── WBP_EquipSlot (RingLeft)
        └── WBP_EquipSlot (RingRight)
```

**장비 슬롯 (WBP_EquipSlot) 구성:**
```
Button
└── Overlay
    ├── Image (SlotBackground)  — 슬롯 빈 상태 배경
    ├── Image (ItemIcon)        — 장착 아이템 아이콘
    └── TextBlock (SlotLabel)   — 슬롯 이름 (예: "무기")
```

### Graph — RefreshFromEquipment 구현

```
Event RefreshFromEquipment
  → GetEquipmentComponent → GetEquippedItems (TMap<EClientEquipSlot, UClientItemInstance*>)
  → 각 슬롯 위젯에 해당 슬롯의 Item 전달
      → null이면 SlotBackground 표시, ItemIcon 숨김
      → 있으면 ItemIcon 표시
```

---

## 4단계: WBP_ConsumableQuickSlot 신규 생성

`Content/TopDown/UI/` 에 새 Widget Blueprint를 생성합니다.
- 이름: `WBP_ConsumableQuickSlot`
- Parent Class: `ClientConsumableQuickSlotWidgetBase`

### Designer 탭 구성

```
WBP_ConsumableQuickSlot
└── HorizontalBox
    ├── Border (Slot1)
    │   └── Overlay
    │       ├── Image (Slot1Icon)
    │       └── TextBlock (Slot1Count)
    └── Border (Slot2)
        └── Overlay
            ├── Image (Slot2Icon)
            └── TextBlock (Slot2Count)
```

### Graph — RefreshFromConsumableQuickSlot 구현

```
Event RefreshFromConsumableQuickSlot
  → GetConsumableQuickSlotComponent
  → GetItemAtSlot(Slot1) → Slot1Icon/Slot1Count 업데이트
  → GetItemAtSlot(Slot2) → Slot2Icon/Slot2Count 업데이트
```

---

## 5단계: BP_TopDownController — 위젯 스폰 및 바인딩

`BP_TopDownController`의 Event Graph BeginPlay에 다음을 추가합니다.

```
[인벤토리 위젯]
Create Widget (WBP_InventoryHUD) → InventoryHUDWidget
  → InventoryHUDWidget → BindInventoryComponent(InventoryComponent)
  → InventoryHUDWidget → Add to Viewport
  → InventoryHUDWidget → Set Visibility (Hidden)  ← I키로 토글

[장비 위젯]
Create Widget (WBP_EquipmentPanel) → EquipmentPanelWidget
  → EquipmentPanelWidget → BindEquipmentComponent(EquipmentComponent)
  → EquipmentPanelWidget → Add to Viewport
  → EquipmentPanelWidget → Set Visibility (Hidden)

[소모품 퀵슬롯 위젯]
Create Widget (WBP_ConsumableQuickSlot) → ConsumableWidget
  → ConsumableWidget → BindConsumableQuickSlotComponent(ConsumableQuickSlotComponent)
  → ConsumableWidget → Add to Viewport
  → ConsumableWidget → Set Visibility (Visible)  ← 항상 표시
```

### 인벤토리 토글 입력 바인딩

`Content/TopDown/Input/` 에서 Input Action `IA_ToggleInventory` 를 생성합니다.
- Value Type: `Digital (bool)`
- 키 바인딩: `I`

`IMC_Default`에 `IA_ToggleInventory → I` 매핑을 추가합니다.

`BP_TopDownController` Event Graph:
```
EnhancedInput → IA_ToggleInventory (Triggered)
  → Branch: InventoryHUDWidget Visibility == Visible
      → True: SetVisibility(Hidden)
      → False: SetVisibility(Visible)
```

---

## 6단계: 테스트 아이템 더미 데이터

실제 서버 연결 전에 에디터에서 동작을 테스트하려면 Blueprint에서 직접 아이템을 생성해야 합니다.

**BP_TopDownController BeginPlay에 더미 추가 예시:**

```
Construct Object (Class: ClientItemInstance) → TestItem
  → TestItem → InitializeFromData(
        StaticData: (ItemTid=1, DisplayName="테스트 검", Category=Weapon,
                     Rarity=Rare, EquipSlot=Weapon, MaxStackCount=1, MaxDurability=100),
        RuntimeData: (ItemKey="item_001", Count=1, Durability=100)
    )
  → InventoryComponent → AddItem(TestItem)
```

소모품 더미:
```
Construct Object (Class: ClientItemInstance) → TestPotion
  → InitializeFromData(
        StaticData: (ItemTid=100, DisplayName="회복 영약(소)", Category=Consumable,
                     MaxStackCount=5, bCanBindToConsumableQuickSlot=true),
        RuntimeData: (ItemKey="item_potion_01", Count=3)
    )
  → ConsumableQuickSlotComponent → AssignConsumable(TestPotion, Slot1)
```

---

## 요약: 에디터 작업 체크리스트

- [ ] `BP_TopDownController`에 5개 컴포넌트 추가
- [ ] `BP_TopDownController` BeginPlay에 초기화 로직 작성
- [ ] `WBP_InventoryHUD` Parent Class를 `ClientInventoryWidgetBase`로 변경 + RefreshFromInventory 구현
- [ ] `WBP_EquipmentPanel` 신규 생성 + RefreshFromEquipment 구현
- [ ] `WBP_ConsumableQuickSlot` 신규 생성 + RefreshFromConsumableQuickSlot 구현
- [ ] `BP_TopDownController`에 위젯 스폰/바인딩 로직 추가
- [ ] `IA_ToggleInventory` Input Action 생성 + `IMC_Default` 매핑
- [ ] 더미 아이템으로 기본 동작 테스트
