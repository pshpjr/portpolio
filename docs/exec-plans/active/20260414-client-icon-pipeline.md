# Exec Plan: 클라이언트 아이콘 자동 임포트 파이프라인

> 생성: 2026-04-14
> 영역: `client/Content/Python/`, `client/tools/icon_staging/`, `client/docs/`, `client/client.uproject`

## 배경

`item_table.data.json`의 각 아이템은 `IconKey` 문자열을 가지지만, 현재 이 키에 대응하는 텍스처 에셋을 수동으로 만들어야 한다. UI 아이콘이 늘어날수록 "PNG를 UE로 가져오고 → 위치·이름을 맞추고 → IconKey 매핑이 틀렸는지 확인"을 사람이 반복해야 한다.

사용자가 원하는 플로우:

- 사용자가 특정 폴더에 PNG를 던지거나 에이전트가 라이선스 확인된 소스에서 다운로드한다.
- 에디터를 기동하면 자동으로 `Content/UI/Icons/` 아래가 최신 상태가 된다.

## 목표

에디터 기동 시 `tools/icon_staging/manifest.json`의 해시 변경을 감지해, 변경된 엔트리만 `Content/UI/Icons/<Category>/T_<IconKey>.uasset`으로 임포트·갱신하는 파이썬 파이프라인을 만든다.

## 범위

- `client.uproject`에 `PythonScriptPlugin`, `EditorScriptingUtilities` 활성화
- `client/Content/Python/init_unreal.py` — 기동 시 manifest 해시 체크 후 import 호출
- `client/Content/Python/import_icons.py` — manifest 읽기 + Texture2D 임포트 + 경로 규약
- `client/tools/icon_staging/manifest.json` — 템플릿
- `client/tools/icon_staging/README.md` — 추가 절차
- `client/tools/sync_icons.bat` — 수동/CI 실행용 백업 트리거
- `client/docs/icon-pipeline.md` — 파이프라인/스키마/문제 해결

## 범위 밖 (추후)

- 커스텀 `UIconRegistryDataAsset` — 지금은 경로 규약(`/Game/UI/Icons/<Category>/T_<IconKey>`)으로 런타임 룩업. 런타임에서 `IconKey → Texture` 매핑 사용이 반복되면 그때 C++ 레지스트리 추가.
- git pre-commit 훅 자동 설치
- 외부 소스 자동 다운로드 크롤러 (라이선스 검증 부담으로 초기 스코프 제외)

## 단계

- [x] 1. exec-plan 등록 및 INDEX 갱신
- [x] 2. `client.uproject`에 PythonScriptPlugin + EditorScriptingUtilities 추가
- [x] 3. `client/tools/icon_staging/` 디렉터리 구조 생성 (manifest 템플릿, README, `incoming/.gitkeep`)
- [x] 4. `Content/Python/import_icons.py` 작성
- [x] 5. `Content/Python/init_unreal.py` 작성 (manifest 해시 저장 위치: `Saved/IconPipeline/last_manifest.sha256`)
- [x] 6. `tools/sync_icons.bat` 작성
- [x] 7. `client/docs/icon-pipeline.md` 작성
- [ ] 8. 실제 엔진에서 실행 검증 (사용자 환경 필요 — 수동)

## 완료 기준

- [ ] 에디터 기동 시 manifest 변경이 있으면 `Output Log`에 `[IconPipeline]` 태그로 변경된 엔트리 이름과 임포트 결과가 찍힌다.
- [ ] manifest 변경이 없을 때 임포트가 스킵되고 로그에 "no changes" 메시지만 남는다.
- [ ] 실패한 엔트리는 전체 파이프라인을 중단시키지 않고 해당 엔트리만 경고 로그로 남긴다.
- [ ] `tools/sync_icons.bat`를 수동 실행하면 manifest 전체를 강제로 재임포트한다.
- [ ] `client/docs/icon-pipeline.md`에 manifest 스키마·추가 절차·경로 규약·문제 해결이 문서화되어 있다.

## 설계 메모

**manifest 스키마 (v1)**

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

- `IconKey`는 `item_table.data.json`의 `IconKey`와 동일 소문자_스네이크케이스.
- `Category`는 폴더 분류용 (`Weapon`, `Armor`, `Consumable`, `Skill`, `UI` 등). 신규 카테고리 추가는 기획 합의 필요.
- `License`가 비어 있거나 화이트리스트 외 값이면 임포트를 스킵하고 경고.

**경로 규약**

- 패키지 경로: `/Game/UI/Icons/<Category>/T_<IconKey>`
- 디스크 경로: `client/Content/UI/Icons/<Category>/T_<IconKey>.uasset`
- 런타임 룩업: `LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/Icons/<Category>/T_<IconKey>"))`

**해시 저장 위치**

- `client/Saved/IconPipeline/last_manifest.sha256` — `Saved/`는 git 무시 경로이므로 사용자별 상태로 남아도 무해. CI는 항상 `--force`로 실행.

## 미해결 사항

- 카테고리 목록을 manifest에 하드코드할지, `item_table.data.json`에서 유추할지. 초기엔 manifest 기반으로 가고 카테고리 수가 안정되면 코드로 상수화.
- 동일 `IconKey` 중복 등록 발생 시 정책 — 초기엔 마지막 엔트리 승으로 경고만 출력.

## 결과 요약

- `client.uproject`에 `PythonScriptPlugin`, `EditorScriptingUtilities`를 Editor 타겟으로 활성화.
- `client/Content/Python/init_unreal.py`: 에디터 기동 시 `tools/icon_staging/manifest.json`의 SHA-256을 `Saved/IconPipeline/last_manifest.sha256`와 비교. 차이가 있으면 `import_icons.run()` 호출. 실패 엔트리가 있으면 해시를 갱신하지 않아 다음 기동에 재시도.
- `client/Content/Python/import_icons.py`: manifest 검증(IconKey 스네이크케이스, 라이선스 화이트리스트, 필수 필드), 중복 키 경고, `AssetImportTask`로 `/Game/UI/Icons/<Category>/T_<IconKey>` 생성·저장. 엔트리별 예외는 로그만 남기고 파이프라인 지속.
- `client/tools/icon_staging/`: 빈 `manifest.json`, README, `incoming/.gitkeep`.
- `client/tools/sync_icons.bat`: `UnrealEditor-Cmd ... -run=pythonscript -script=import_icons.py --force` 래퍼. `UE_EDITOR_CMD` 환경 변수로 엔진 경로 오버라이드.
- `client/docs/icon-pipeline.md`: 플로우 다이어그램, manifest 스키마, 경로 규약, 로그 해석, 문제 해결, 추후 확장.
- 사용자 환경 검증 단계(엔진 기동)는 남아 있음.
- 후속 수정: `InventorySlot::SetItem`이 `/Game/UI/Icons/<IconKey>.png`를 잘못된 경로로 찾던 것을 `EItemCategory` enum을 사용한 `/Game/UI/Icons/<Category>/T_<IconKey>.T_<IconKey>`로 교체. `import_icons.py`의 Category 화이트리스트를 `EItemCategory` enum과 1:1로 맞추고, 문서/README의 카테고리 목록을 실제 enum (`SkillBook` 등)으로 정정.
