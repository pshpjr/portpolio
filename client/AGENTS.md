# AGENTS.md
> 클라이언트 앱용 에이전트 진입점입니다.

---

## 프로젝트 요약

Unreal Engine 5 기반 게임 클라이언트. 1~4인 인스턴스 던전 RPG의 프론트엔드를 담당한다.

상위 제품 기획과 공용 문서는 루트 [`../docs/`](../docs/) 아래에서 관리한다.

---

## 핵심 원칙

- **C++ 우선**: Blueprint로만 가능한 요소(위젯 레이아웃, 애니메이션 몽타주 할당, 머티리얼 등)를 제외한 모든 로직은 C++로 구현한다.
- Blueprint 서브클래스는 데이터 할당(에셋 참조, 기본값 설정)용으로만 사용한다.
- 게임플레이 로직, 입력 처리, 상태 관리는 반드시 C++ 클래스에 둔다.

---

## 디렉터리 구조

```
client/
  Source/client/
    Public/          — 헤더 파일
    Private/         — 구현 파일
    Public/UI/       — UI 위젯 헤더
    Private/UI/      — UI 위젯 구현
    Public/Inventory/ — 인벤토리/장비 헤더
    Private/Inventory/ — 인벤토리/장비 구현
    Public/Generated/ — 자동 생성 코드 (수동 편집 금지)
```
