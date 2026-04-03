# Exec Plan: 클라이언트 인벤토리 빌드 충돌 해소

## 목표

`client-inventory-foundation`에서 확인된 Marketplace 플러그인 간 `ItemDataAsset.h` 헤더 충돌을 해소하여
인벤토리 초안 코드가 실제로 빌드 통과할 수 있는 상태를 만든다.

## 범위

- `client/` 플러그인 설정 또는 빌드 규칙
- 충돌 원인이 되는 Marketplace 플러그인 2개의 헤더 충돌 지점
- `client/Source/client/client.Build.cs` (필요 시)

## 완료 기준

- [ ] 빌드 시 `ItemDataAsset.h` 중복 오류가 사라진다
- [ ] `Build.bat clientEditor Win64 Development` 빌드가 통과한다
- [ ] 인벤토리 컴포넌트 코드가 UHT 오류 없이 컴파일된다

## 진행 상황

- [ ] 충돌 원인 플러그인 식별
- [ ] 충돌 해소 방법 결정 (플러그인 비활성화 / 헤더 경로 격리 / 기타)
- [ ] 수정 적용 및 빌드 검증

## 설계 결정

- `client/` 전용 AGENTS.md와 exec-plan 체계 신설은 이 plan의 범위 밖이다. 별도 plan으로 분리한다.
- 충돌 해소 방법은 Marketplace 플러그인 코드 수정보다 프로젝트 설정 수준에서 해결하는 것을 우선한다.
