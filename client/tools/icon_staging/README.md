# Icon Staging

UI 아이콘을 UE 에셋으로 들이는 입구입니다. 에디터 기동 시 `manifest.json` 해시가 바뀌었으면 자동으로 임포트됩니다.

## 추가 절차

1. PNG 파일을 `incoming/` 아래에 복사합니다.
2. `manifest.json`의 `entries`에 항목을 추가합니다.
   ```json
   {
     "IconKey": "weapon_sword_shield",
     "Category": "Weapon",
     "Source": "incoming/sword_shield_01.png",
     "License": "CC0",
     "SourceUrl": "https://game-icons.net/...",
     "Notes": ""
   }
   ```
3. 에디터를 기동(또는 재시작)합니다.
4. `Output Log`의 `[IconPipeline]` 라인으로 결과를 확인합니다.

## 필드 규약

- `IconKey`: `item_table.data.json`의 `IconKey`와 동일한 소문자 스네이크케이스.
- `Category`: 폴더 분류 (`Weapon`, `Armor`, `Consumable`, `SkillBook`, `Material`, `Quest`, `Currency` (런타임 `EItemCategory`와 동일)). 신규 카테고리는 기획과 합의.
- `Source`: 이 디렉터리 기준 상대 경로.
- `License`: `CC0`, `CC-BY`, `CC-BY-SA`, `Custom` 중 하나. 비어 있으면 임포트 스킵.
- `SourceUrl`: 출처 URL. 사용자 제공 파일이면 비워도 됨.
- `Notes`: 라이선스 표기 요구사항이나 편집 이력 등 자유 기록.

## 결과물 위치

- 패키지 경로: `/Game/UI/Icons/<Category>/T_<IconKey>`
- 디스크 경로: `client/Content/UI/Icons/<Category>/T_<IconKey>.uasset`

## 강제 재임포트

해시 캐시를 무시하고 전체를 다시 임포트하려면 저장소 루트에서:

```
client\tools\sync_icons.bat
```

## 주의

- `incoming/`의 원본 PNG는 git에 포함됩니다. 라이선스가 배포 가능한 것만 올리세요.
- 라이선스가 불분명한 파일은 이 디렉터리에 두지 말고 별도 위치에 격리한 뒤 검증 후 옮깁니다.
