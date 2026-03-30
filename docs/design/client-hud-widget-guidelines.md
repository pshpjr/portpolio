# Client HUD / Widget Best Practices

## 목적

언리얼 클라이언트에서 HUD와 여러 `UUserWidget` 베이스를 함께 운용할 때,
구조가 빠르게 엉키지 않도록 책임 분리와 계층 기준을 정리한다.

이 문서는 특히 아래처럼 성격이 다른 UI를 동시에 다룰 때의 기본 원칙을 다룬다.

- 체력, 마나, 스태미나, 아이덴티티 게이지 같은 전투 리소스 표시
- 전투 중 항상 노출되는 스킬 핫바
- 캐릭터 정보, 장비 비교, 성장 수치를 보는 캐릭터 정보 화면
- 스킬 배치, 강화, 프리셋을 다루는 스킬트리 화면
- 토글해서 여닫는 인벤토리 화면
- 인벤토리와 함께 열릴 수 있는 장비 화면
- 별도 접근 흐름을 가지는 창고 화면
- 진행 중 목표를 보여주는 퀘스트 화면과 추적 UI
- 장기 과제를 보여주는 업적 화면
- 비동기 보상/수령 흐름이 필요한 메일 화면

---

## 한 줄 원칙

`AHUD` 또는 `APlayerController` 하나가 여러 위젯을 관리하고, 각 위젯은 자기 화면 책임만 가진다.

즉, 보통의 권장 구조는 아래와 같다.

- `HUD` 하나
- 최상위 `UUserWidget` 여러 개
- 각 위젯 아래의 서브위젯 여러 개

`HUD`를 여러 개 두는 방식보다 훨씬 단순하고, 화면 전환과 상태 공유도 쉬워진다.

---

## 왜 HUD 하나가 기본인가

언리얼에서 HUD는 “현재 로컬 플레이어의 UI 루트 관리자”에 가깝다.
실제 게임 화면은 여러 위젯이 동시에 존재하더라도, 이를 소유하고 표시 순서를 정하고 입력 모드를 바꾸는 주체는 하나인 편이 안정적이다.

HUD가 여러 개가 되면 아래 문제가 생기기 쉽다.

- 어떤 HUD가 어떤 위젯을 소유하는지 흐려진다.
- 입력 모드 전환 책임이 분산된다.
- 동일한 플레이어 상태를 여러 군데서 중복 참조하게 된다.
- `Viewport` 추가/제거 순서가 꼬이기 쉽다.

반대로 HUD 하나가 여러 위젯을 관리하면 아래 장점이 있다.

- UI 생성 시점이 한곳에 모인다.
- Z-order, 열림/닫힘 정책을 한곳에서 통제할 수 있다.
- 플레이어 인벤토리 컴포넌트, 장비 컴포넌트, 핫바 컴포넌트 바인딩 위치가 명확해진다.
- 화면 전환 규칙을 코드와 블루프린트 모두에서 이해하기 쉬워진다.

---

## 권장 계층 구조

기본 계층은 아래처럼 나누는 것을 권장한다.

```text
Local Player
└─ PlayerController or HUD
   ├─ Always-on HUD widgets
   │  ├─ StatusBarsWidget
   │  └─ SkillHotbarWidget
   ├─ Toggleable screen widgets
   │  ├─ InventoryScreenWidget
   │  ├─ StorageScreenWidget
   │  ├─ CharacterMenuWidget
   │  ├─ SkillTreeScreenWidget
   │  ├─ QuestScreenWidget
   │  ├─ AchievementScreenWidget
   │  └─ MailScreenWidget
   └─ Optional popup widgets
      ├─ ItemTooltipWidget
      ├─ ConfirmationDialogWidget
      └─ ErrorToastWidget
```

여기서 중요한 점은 “모든 UI를 같은 레벨로 올리지 않는다”는 것이다.

- 항상 보여야 하는 UI는 `Always-on HUD widgets`
- 메뉴 성격의 UI는 `Toggleable screen widgets`
- 툴팁, 다이얼로그, 토스트는 `Popup widgets`

이렇게 성격별로 나누면 UI가 커져도 유지가 쉽다.

---

## 여러 위젯 베이스를 두는 것이 좋은 경우

다음 조건이면 위젯 베이스를 여러 개 두는 편이 좋다.

- 표시 위치와 생명주기가 다르다.
- 사용하는 데이터 소스가 다르다.
- 갱신 빈도가 다르다.
- 입력 처리 방식이 다르다.
- 블루프린트 담당자가 화면 단위로 작업하기 원한다.

현재 인벤토리 계층에서는 아래 분리가 자연스럽다.

- `ClientSkillHotbarWidgetBase`
  - 전투 중 상시 표시
  - 짧은 주기로 갱신될 수 있음
  - 슬롯 강조, 쿨다운, 입력 힌트 표시
- `StatusBarsWidgetBase`
  - HP/MP/스태미나/궁극기 게이지 같은 전투 자원 표시
  - 피격, 회복, 자원 소모처럼 빈번한 변화를 빠르게 반영
  - 숫자, 바, 경고 효과를 함께 표현
- `ClientInventoryWidgetBase`
  - 인벤토리 슬롯 목록 표시
  - 드래그 앤 드롭과 정렬 중심
- `ClientEquipmentWidgetBase`
  - 장비 슬롯 표시
  - 캐릭터 프리뷰 또는 스탯 패널과 자주 함께 배치
- `CharacterInfoWidgetBase`
  - 전투력 계산에 쓰이는 주요 스탯과 성장 정보를 표시
  - 장비 변경에 따른 능력치 차이를 즉시 반영
  - 레벨, 클래스, 무기 타입, 핵심 전투 수치를 보여줌
- `SkillTreeWidgetBase`
  - 습득한 스킬 목록, 강화 단계, 분기 선택, 프리셋을 표시
  - 스킬 상세 툴팁, 요구 조건, 자원 소모 정보를 함께 제공
- `ClientStorageWidgetBase`
  - 공유 창고 탭과 슬롯 표시
  - 마을/NPC 상호작용과 결합되기 쉬움

이 분리는 이미 현재 코드 구조와도 잘 맞는다.

- `client/Source/client/Public/UI/ClientInventoryWidgetBase.h`
- `client/Source/client/Public/UI/ClientEquipmentWidgetBase.h`
- `client/Source/client/Public/UI/ClientStorageWidgetBase.h`
- `client/Source/client/Public/UI/ClientSkillHotbarWidgetBase.h`

---

## 위젯을 무조건 최상위로 두면 안 되는 이유

모든 화면을 전부 HUD 직속 최상위 위젯으로 두면 초반에는 편해 보여도,
중간 규모부터는 UI 관계가 오히려 더 복잡해진다.

예를 들어 인벤토리와 장비는 논리적으로 강하게 연결되어 있다.
유저는 보통 인벤토리를 열 때 장비 비교나 장착도 함께 본다.
이 경우 아래처럼 구성하는 편이 더 낫다.

```text
InventoryScreenWidget
├─ InventoryPanel
├─ EquipmentPanel
├─ ItemDetailPanel
└─ CurrencyPanel
```

즉, 개별 기능용 위젯 베이스는 유지하되 “사용자 경험 상 하나의 화면”이라면 상위 컨테이너 위젯 아래로 묶는 것이 좋다.

권장 기준은 아래와 같다.

- 항상 독립적으로 뜨고 꺼지는가
  - 독립 최상위 위젯 후보
- 같은 화면 안에서 항상 같이 보이는가
  - 같은 화면 컨테이너 아래 서브위젯 후보
- UI 디자이너가 별도 블루프린트로 다루고 싶은가
  - 서브위젯으로 분리
- 데이터 소스는 같지만 표현 영역만 다른가
  - 같은 화면 아래 여러 패널로 분리

---

## 추천 책임 분리

### `AHUD` 또는 `APlayerController`

이 계층은 UI의 “운영 책임”을 가진다.

담당할 일:

- 위젯 클래스 로드
- 위젯 생성과 보관
- `AddToViewport` / `RemoveFromParent`
- 화면 열기/닫기 정책
- 입력 모드 전환
- 마우스 커서 표시 여부
- 공통 컴포넌트 참조 주입

가급적 넣지 말아야 할 일:

- 인벤토리 정렬 로직
- 아이템 이동 규칙
- 장착 가능 여부 계산
- 슬롯 표시 세부 규칙

즉, HUD는 “화면 오케스트레이션”까지 맡고, 실제 게임 규칙은 아래 계층으로 내려보내는 게 좋다.

### `UActorComponent`

이 계층은 UI가 보여줄 도메인 상태를 가진다.

현재 구조 기준 예:

- `UClientInventoryComponent`
- `UClientEquipmentComponent`
- `UClientStorageComponent`
- `UClientSkillHotbarComponent`
- 추후 추가 후보:
  - `UClientAttributeComponent`
  - `UClientCharacterProfileComponent`
  - `UClientSkillTreeComponent`
  - `UClientQuestComponent`
  - `UClientAchievementComponent`
  - `UClientMailComponent`

담당할 일:

- 아이템/스킬 상태 저장
- 슬롯 이동, 장착, 등록 같은 규칙 처리
- 상태 변경 delegate 발생

장점:

- UI가 바뀌어도 게임 상태 코드는 유지된다.
- 나중에 서버 동기화가 붙어도 UI와 직접 결합되지 않는다.

### `UUserWidget`

이 계층은 “표현 책임”을 가진다.

담당할 일:

- 슬롯 그리기
- 버튼 상태 갱신
- 툴팁 표시
- 드래그 앤 드롭 UI 처리
- delegate 구독 후 화면 갱신

가급적 넣지 말아야 할 일:

- 장비 가능 여부의 최종 규칙 결정
- 스택 병합 규칙의 최종 판단
- 서버 패킷 조립

---

## 바인딩 방식 베스트 프랙티스

위젯은 컴포넌트를 직접 소유하지 말고, 참조를 바인딩받는 방식이 좋다.

현재 베이스 위젯 구조도 이 방향을 따른다.

- `BindInventoryComponent()`
- `BindEquipmentComponent()`
- `BindStorageComponent()`
- `BindSkillHotbarComponent()`

권장 흐름:

1. HUD가 위젯을 생성한다.
2. HUD가 플레이어의 관련 컴포넌트를 찾는다.
3. HUD가 각 위젯에 컴포넌트를 바인딩한다.
4. 위젯은 delegate를 구독한다.
5. 상태 변경 시 `RefreshFrom...()` 블루프린트 이벤트를 호출한다.

이 방식의 장점:

- 위젯 재사용이 쉽다.
- 테스트 더블이나 임시 데이터 바인딩이 쉽다.
- 플레이어 교체, 재접속, 화면 재생성에 대응하기 쉽다.

---

## 입력 키와 슬롯 enum을 분리해야 하는 이유

스킬 슬롯 enum을 `Q`, `E`, `R`, `F` 같은 입력키 이름으로 두면 처음에는 직관적이지만,
조금만 시스템이 커져도 문제가 생긴다.

문제 예:

- 키 리바인딩을 넣으면 enum 이름이 의미를 잃는다.
- 게임패드 대응 시 같은 슬롯을 다른 버튼으로 매핑해야 한다.
- UI는 “첫 번째 액티브 슬롯”을 말하고 싶은데 enum은 특정 키 이름을 강제한다.
- 서버나 저장 데이터가 입력 디바이스 개념에 오염된다.

그래서 슬롯 enum은 의미 중심으로 두는 편이 좋다.

권장 예:

- `Active1`
- `Active2`
- `Active3`
- `Active4`
- `Identity`

그리고 입력 매핑은 별도 계층에서 연결한다.

예:

- 키보드 `Q` -> `Active1`
- 키보드 `E` -> `Active2`
- 게임패드 `RightTrigger + FaceTop` -> `Active1`

즉:

- 슬롯 enum은 “도메인 상태”
- 입력 바인딩은 “조작 체계”

둘은 분리하는 것이 장기적으로 안전하다.

---

## 현재 인벤토리/장비/창고/핫바에 대한 추천 화면 구성

---

## 캐릭터 정보 UI 권장 구조

캐릭터 정보 화면은 단순 스탯 표가 아니라 “현재 성장 상태 요약 + 장비 영향 확인 + 빌드 이해”를 돕는 화면이어야 한다.

권장 구조:

```text
CharacterMenuWidget
├─ CharacterSummaryPanel
├─ EquipmentPanel
├─ PrimaryStatsPanel
├─ CombatStatsPanel
└─ GrowthInfoPanel
```

권장 표시 대상:

- 캐릭터 이름, 클래스, 레벨
- 현재 장착 무기 타입
- HP, MP, 공격력, 방어력 같은 핵심 능력치
- 치명타, 공격 속도, 쿨다운 감소, 무력화 보정 같은 전투 세부 수치
- 장비 교체 시 증감 비교

이 화면은 장비 화면과 강하게 연결되므로 아래 두 방식 중 하나가 좋다.

- `CharacterMenuWidget` 아래에 `EquipmentPanel`을 포함
- `InventoryScreenWidget`과 별개로 두되 장비 비교 UI를 공유

권장 원칙:

- “현재값”과 “변화량”을 구분해 보여준다.
- 스탯 종류가 많아져도 핵심과 상세를 분리한다.
- 전투에 직접 체감되는 수치는 상단 또는 우선 영역에 둔다.

주의:

- 캐릭터 정보 위젯이 스탯 계산을 직접 하면 안 된다.
- 계산은 캐릭터/속성 컴포넌트 계층에서 끝내고, UI는 표시만 담당한다.

---

## 스킬트리 UI 권장 구조

스킬트리 화면은 단순 목록이 아니라 “습득 상태, 성장 선택, 프리셋 운용”을 함께 다루는 화면으로 설계하는 것이 좋다.

권장 구조:

```text
SkillTreeScreenWidget
├─ WeaponTabBar
├─ SkillListPanel
├─ SkillDetailPanel
├─ SkillUpgradePanel
└─ SkillPresetPanel
```

권장 표시 대상:

- 무기 타입별 스킬 카테고리
- 습득 여부
- 현재 스킬 레벨
- 다음 강화 비용 또는 필요 자원
- 분기 선택 또는 변형 선택
- 핫바 등록 상태
- 프리셋 저장/불러오기

스킬트리 화면과 핫바는 분리하되 서로 연결되어야 한다.

- `SkillTreeScreenWidget`
  - 전체 스킬 탐색, 강화, 설명 확인
- `SkillHotbarWidget`
  - 현재 전투에 사용할 슬롯 배치 확인

권장 상호작용:

- 스킬트리에서 선택한 스킬을 바로 핫바 슬롯에 보낼 수 있게 한다.
- 현재 핫바 등록 중인 스킬은 트리에서 시각적으로 표시한다.
- 무기 탭 변경 시 해당 무기 스킬군만 필터링한다.

주의:

- 스킬 슬롯 enum과 입력키를 섞지 않는다.
- 트리 화면은 “어떤 스킬을 Active1에 넣을지”는 보여줘도, `Q` 키를 직접 모델링하지 않는다.

---

## 체력 및 소모 자원 표시에 대한 권장 구조

전투 자원 UI는 메뉴형 화면과 다르게 “항상 보이는 HUD 계층”으로 두는 것이 좋다.

권장 표시 대상:

- HP
- MP 또는 클래스 자원
- 스태미나 또는 회피 자원
- 무기별 아이덴티티 게이지
- 버프/디버프 핵심 상태
- 소비성 전투 자원

권장 구조:

```text
CombatHudRoot
├─ StatusBarsWidget
├─ SkillHotbarWidget
├─ BuffDebuffWidget
└─ PartyStatusWidget
```

`StatusBarsWidget`는 별도 위젯으로 분리하는 편이 좋다.

이유:

- 체력/자원은 핫바보다 갱신 빈도가 더 높을 수 있다.
- 스킬 핫바와 시각적 역할이 다르다.
- 파티 상태, 보스 상태, 타깃 상태 표시와도 연동되기 쉽다.

표현 원칙:

- 숫자와 바를 같이 제공해 직관성과 정확도를 함께 준다.
- 위험 구간에서는 색상, 점멸, 진동 같은 경고 연출을 넣는다.
- 소모 자원은 “현재값 변화 방향”이 보이게 즉시 반영한다.
- 실시간 값은 이벤트 기반 갱신, 애니메이션은 별도 보간으로 처리한다.

주의:

- 위젯이 직접 스탯 계산을 하지 않는다.
- 위젯은 이미 계산된 현재값/최대값/비율만 받아 표시한다.
- 리소스 바의 색상 규칙과 임계치 경고는 UI 설정값으로 분리하는 편이 좋다.

### 전투 중 HUD

상시 노출:

- `StatusBarsWidget`
- `SkillHotbarWidget`
- HP/MP/게이지
- 파티 프레임
- 상태 이상/버프 표시

숨김 또는 축약 가능:

- 소형 퀵 인벤토리
- 미니 알림

### 인벤토리 화면

추천:

```text
InventoryScreenWidget
├─ InventoryWidget
├─ EquipmentWidget
├─ ItemDetailsWidget
└─ CurrencySummaryWidget
```

이유:

- 아이템 선택과 장비 비교가 같은 맥락에서 자주 일어난다.
- 하나의 토글 입력으로 열고 닫기 좋다.
- 마우스 중심 조작 화면으로 묶기 쉽다.

### 창고 화면

추천:

```text
StorageScreenWidget
├─ StorageWidget
├─ InventoryWidget
└─ ItemTransferControls
```

이유:

- 창고는 항상 인벤토리와 비교 이동이 필요하다.
- 공유 창고 탭과 캐릭터 인벤토리를 동시에 보여주는 편이 사용성이 좋다.

즉, `InventoryWidget`은 독립 위젯이면서도 `InventoryScreen`과 `StorageScreen` 양쪽에서 재사용될 수 있어야 한다.

---

## 퀘스트 UI 권장 구조

퀘스트는 보통 “항상 노출되는 소형 추적 UI”와 “상세 목록 화면”을 분리하는 편이 좋다.

권장 구조:

```text
Hud
├─ QuestTrackerWidget
└─ ...

QuestScreenWidget
├─ QuestCategoryList
├─ QuestListPanel
├─ QuestDetailPanel
└─ RewardPreviewPanel
```

분리 이유:

- 플레이 중에는 현재 목표 몇 개만 빠르게 보고 싶다.
- 상세 보상, 서브 목표, 서사 텍스트는 별도 화면에서 보는 편이 낫다.

권장 책임:

- `QuestTrackerWidget`
  - 현재 추적 중인 퀘스트만 요약 표시
  - 완료 가능 상태, 새 목표 도달 같은 짧은 피드백
- `QuestScreenWidget`
  - 전체 퀘스트 목록
  - 카테고리/지역/진행 상태별 필터
  - 상세 목표와 보상 표시

추가 권장:

- 새 퀘스트 수락, 목표 갱신, 완료 가능 상태는 토스트나 작은 알림으로 별도 처리
- HUD 추적 목록은 너무 길지 않게 제한

---

## 업적 UI 권장 구조

업적은 전투 HUD보다 “메뉴형 진행 화면”에 가깝다.

권장 구조:

```text
AchievementScreenWidget
├─ CategoryTree
├─ AchievementList
├─ AchievementDetailPanel
└─ ProgressSummaryPanel
```

업적은 보통 아래 특성을 가진다.

- 실시간 확인 빈도는 낮다.
- 누적 진행률과 보상 확인이 중요하다.
- 카테고리 탐색이 필요하다.

그래서 HUD 상시 노출보다는 메뉴 화면 배치가 적합하다.

예외적으로 HUD에 둘 수 있는 요소:

- 최근 달성 업적 토스트
- 특정 업적 진행률의 한시적 추적

주의:

- 퀘스트와 업적은 비슷해 보여도 목적이 다르다.
- 퀘스트는 현재 플레이 목표 중심
- 업적은 장기 누적 성취 중심

따라서 같은 컴포넌트나 같은 화면으로 억지 통합하지 않는 편이 좋다.

---

## 메일 UI 권장 구조

메일은 “비동기 보상함 + 메시지 보관함” 성격이 강하므로, 별도 화면으로 분리하는 것이 좋다.

권장 구조:

```text
MailScreenWidget
├─ MailFolderTabs
├─ MailListPanel
├─ MailDetailPanel
└─ AttachmentClaimPanel
```

메일이 별도 화면이어야 하는 이유:

- 읽음/안읽음 상태가 있다.
- 첨부 보상 수령 흐름이 있다.
- 수령 만료 시간, 보관 기간, 시스템/운영 메일 구분이 필요할 수 있다.

HUD에서 필요한 것은 전체 메일 UI가 아니라 “상태 알림” 정도다.

예:

- 신규 메일 아이콘 배지
- 보상 미수령 메일 개수
- 기간 만료 임박 알림

권장 분리:

- HUD: 새 메일 여부, 미수령 개수
- MailScreen: 목록, 본문, 첨부 수령, 일괄 수령

---

## 메뉴형 화면 분류 권장안

아래처럼 화면군을 나누면 구조가 훨씬 선명해진다.

### 상시 HUD

- `StatusBarsWidget`
- `SkillHotbarWidget`
- `QuestTrackerWidget`
- 파티/버프/알림 표시

### 캐릭터/전투 준비 화면

- 인벤토리
- 장비
- 캐릭터 정보
- 스킬트리
- 스킬 상세/프리셋

### 진행/수집 화면

- 퀘스트
- 업적
- 컬렉션
- 코덱스

### 비동기 수령/커뮤니케이션 화면

- 메일
- 시스템 보상함
- 공지/운영 메시지

이 분류를 먼저 정한 뒤 HUD와 메뉴 네비게이션을 설계하면, 화면이 늘어나도 분기점이 명확하다.

---

## HUD와 PlayerController 중 어디서 관리할까

둘 다 가능하지만, 기본 판단 기준은 아래처럼 잡는 것이 좋다.

### `AHUD` 중심

적합한 경우:

- UI 생성과 레이어 관리가 중심 관심사다.
- 전형적인 HUD 패턴을 따르고 싶다.
- 화면 관리 코드를 컨트롤 입력 코드와 분리하고 싶다.

### `APlayerController` 중심

적합한 경우:

- 입력 처리와 UI 토글이 강하게 연결된다.
- 현재 프로젝트가 HUD보다 Controller 중심 구조를 선호한다.
- 초안 단계에서 단순하게 시작하고 싶다.

실무적으로는 아래도 많이 쓴다.

- `PlayerController`가 토글 입력을 받는다.
- `HUD`가 실제 위젯 생성/표시를 담당한다.

중요한 것은 어느 클래스를 고르느냐보다, “한 곳에서 총괄한다”는 일관성이다.

---

## 상태 갱신 방식 권장안

UI 갱신은 매 프레임 폴링보다 이벤트 기반을 우선한다.

권장 방식:

- 컴포넌트에서 상태 변경
- delegate broadcast
- 위젯이 구독 후 `RefreshFrom...()` 호출

이점:

- 불필요한 Tick 감소
- 상태 변경 지점 추적이 쉬움
- UI 반응 타이밍이 명확함

Tick 기반 갱신은 아래처럼 제한적으로만 쓴다.

- 드래그 중 임시 하이라이트
- 쿨다운 애니메이션 보간
- 마우스 추적 툴팁 위치 보정

---

## 피해야 할 구조

### 1. 위젯이 게임 규칙을 직접 소유하는 구조

문제:

- UI와 게임 로직이 강결합된다.
- 화면이 바뀌면 규칙도 함께 건드려야 한다.

### 2. 모든 패널을 HUD 직속으로만 나열하는 구조

문제:

- 화면 관계가 커질수록 관리 포인트가 늘어난다.
- 인벤토리/장비/창고처럼 연결된 화면 묶음이 흐려진다.

### 3. 입력키 이름을 도메인 enum으로 쓰는 구조

문제:

- 리바인딩과 멀티 디바이스 대응이 어려워진다.

### 4. 위젯이 플레이어를 직접 탐색하며 필요한 데이터를 다 찾아가는 구조

문제:

- 참조 경로가 숨겨진다.
- 테스트와 재사용이 어려워진다.

HUD 또는 화면 루트가 명시적으로 의존성을 주입하는 편이 낫다.

---

## 추천 구현 순서

1. `HUD` 또는 `PlayerController`에 UI 관리자 책임을 확정한다.
2. 상시 HUD 위젯과 메뉴 위젯을 구분한다.
3. 인벤토리/장비/창고/핫바 컴포넌트를 데이터 소스로 고정한다.
4. 각 기능 위젯 베이스는 바인딩 + refresh 이벤트만 가진다.
5. 인벤토리/장비를 묶는 화면 컨테이너 위젯을 추가한다.
6. 창고 화면은 인벤토리 재사용 구조로 만든다.
7. 입력 매핑은 슬롯 enum과 분리해서 연결한다.

---

## 현재 저장소 기준 권장 방향

현재 구조를 기준으로는 아래 방향이 가장 무난하다.

- `ClientHUD` 또는 `ClientPlayerController` 하나가 UI 루트를 관리
- `StatusBarsWidgetBase`와 `ClientSkillHotbarWidgetBase`는 상시 HUD 위젯
- `QuestTrackerWidget`은 상시 HUD 보조 위젯
- `ClientInventoryWidgetBase`, `ClientEquipmentWidgetBase`는 `InventoryScreenWidget` 아래 패널
- `ClientStorageWidgetBase`는 `StorageScreenWidget` 아래 패널
- `CharacterInfoWidgetBase`는 `CharacterMenuWidget`의 핵심 패널
- `SkillTreeWidgetBase`는 `SkillTreeScreenWidget`의 핵심 패널
- `QuestScreenWidget`, `AchievementScreenWidget`, `MailScreenWidget`은 메뉴형 최상위 화면
- 각 위젯은 `Client...Component`에 바인딩되어 delegate 기반으로 갱신
- 슬롯 enum은 입력키가 아니라 의미 중심 슬롯명 유지

---

## 요약

- HUD는 하나, 위젯은 여러 개가 기본이다.
- 체력과 소모 자원은 상시 HUD 위젯으로 분리하는 편이 좋다.
- 기능 위젯은 분리하되, 사용자 경험상 하나의 화면이면 컨테이너 위젯 아래로 묶는다.
- 캐릭터 정보와 스킬트리는 전투 준비/성장 화면군으로 분리하는 편이 좋다.
- 퀘스트는 추적 HUD와 상세 화면을 분리한다.
- 업적과 메일은 메뉴형 화면으로 분리하고, HUD에는 상태 알림만 둔다.
- 게임 규칙은 컴포넌트에 두고, 위젯은 표현과 상호작용에 집중한다.
- 슬롯 enum은 입력키명이 아니라 의미 기반 이름을 사용한다.
- 갱신은 폴링보다 delegate 기반 이벤트를 우선한다.
