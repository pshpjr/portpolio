# Icon Import Pipeline

UI 아이콘 PNG를 UE 텍스처 에셋으로 들이는 파이프라인 문서입니다. 에디터 기동 시 manifest 변경을 감지해 자동으로 임포트합니다.

---

## 플로우

```
사용자/에이전트가 PNG 배치
    └─ client/tools/icon_staging/incoming/

사용자/에이전트가 manifest.json에 엔트리 추가
    └─ client/tools/icon_staging/manifest.json

에디터 기동
    └─ Content/Python/init_unreal.py 가 manifest 해시 비교
        └─ 변경 시 import_icons.py 호출
            └─ Content/UI/Icons/<Category>/T_<IconKey>.uasset 생성/갱신
        └─ Saved/IconPipeline/last_manifest.sha256 에 해시 기록 (실패 0일 때만)
```

---

## Manifest 스키마 (v1)

```json
{
  "version": 1,
  "entries": [
    {
      "IconKey": "weapon_sword_shield",
      "Category": "Weapon",
      "Source": "incoming/sword_shield_01.png",
      "License": "CC0",
      "SourceUrl": "https://game-icons.net/...",
      "Notes": ""
    }
  ]
}
```

| 필드 | 규약 |
|---|---|
| `IconKey` | 소문자 스네이크케이스 영숫자. `item_table.data.json`의 `IconKey`와 동일. |
| `Category` | 폴더 분류. `Weapon`, `Armor`, `Consumable`, `SkillBook`, `Material`, `Quest`, `Currency` (런타임 `EItemCategory`와 동일) 중 하나. 신규 추가는 기획 합의 필요. |
| `Source` | `client/tools/icon_staging/` 기준 상대 경로. |
| `License` | `CC0`, `CC-BY`, `CC-BY-SA`, `Custom`. 비어 있거나 화이트리스트 외 값이면 스킵. |
| `SourceUrl` | 출처 URL. 사용자 직접 제공이면 비워도 됨. |
| `Notes` | 표기 요구사항, 편집 메모 등 자유 기록. |

---

## 경로 규약

- 패키지: `/Game/UI/Icons/<Category>/T_<IconKey>`
- 디스크: `client/Content/UI/Icons/<Category>/T_<IconKey>.uasset`
- 런타임 룩업: `LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/Icons/<Category>/T_<IconKey>"))`

초기에는 경로 규약만으로 룩업합니다. 런타임에서 `IconKey → Texture` 매핑 사용이 반복되면 `UIconRegistryDataAsset` C++ 레지스트리를 추가합니다 (별도 작업).

---

## 수동 실행

해시 캐시를 무시하고 전체를 강제로 재임포트:

```
client\tools\sync_icons.bat
```

`UE_EDITOR_CMD` 환경 변수로 UE 설치 경로를 지정할 수 있습니다. 기본값은 `C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe`.

---

## 로그 확인

모든 파이프라인 로그는 `[IconPipeline]` 태그로 `Output Log`에 찍힙니다.

- `manifest unchanged, no import needed` — 해시 동일, 임포트 스킵.
- `manifest changed (was=..., now=...), running import` — 해시 차이 감지, 임포트 실행.
- `imported <source> -> <package>` — 엔트리별 성공.
- `entry '<key>' skipped: <reason>` — 검증 실패. 해시 캐시에 기록 안 됨 → 다음 기동에 재시도.
- `done: N succeeded, M failed` — 최종 요약.

---

## 문제 해결

**에디터 기동해도 로그가 안 찍힘**
- `client.uproject`의 `PythonScriptPlugin`, `EditorScriptingUtilities`가 `Enabled: true`인지 확인.
- `Edit → Plugins`에서 Scripting → Python Editor Script Plugin이 켜졌는지 확인.
- `Project Settings → Plugins → Python`에서 "Developer Mode" 및 시작 스크립트 경로 확인.

**임포트는 됐지만 해시가 저장 안 됨**
- 실패 엔트리가 하나라도 있으면 의도적으로 해시를 기록하지 않습니다 (다음 기동에 재시도). 로그에서 실패 원인 확인.

**런타임에서 텍스처가 null**
- 패키지 경로 오타가 가장 흔한 원인. 에셋 브라우저에서 실제 경로를 복사해 비교.
- `Category` 대소문자까지 정확히 일치해야 합니다.

**수동 실행 시 "UnrealEditor-Cmd not found"**
- `UE_EDITOR_CMD` 환경 변수를 설정하거나 `tools/sync_icons.bat`의 기본 경로를 수정.

---

## 추후 확장

- `UIconRegistryDataAsset` C++ 타입 추가: 경로 규약 대신 타입 안전 룩업.
- 카테고리별 압축 설정(UserInterface2D, NoMipMaps 등)을 manifest 또는 카테고리 프리셋으로 지정.
- 라이선스 고지문 자동 수집 → `docs/licenses/icons.md` 생성.
- git pre-commit 훅: `manifest.json` 변경 시 `Content/UI/Icons/` 결과물 누락 감지.
