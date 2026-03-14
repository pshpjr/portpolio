# 에이전트 작업 흐름 (agent-workflow.md)

## 작업 시작 체크리스트

새 세션에서 작업을 시작할 때:

```
□ AGENTS.md 읽기
□ docs/exec-plans/active/ 확인
  → 진행 중인 플랜이 있으면: 해당 파일을 읽고 컨텍스트 복원
  → 없으면: 새 플랜 파일 작성 후 시작
□ 관련 도메인 문서 읽기
□ ARCHITECTURE.md 레이어 규칙 재확인
```

---

## 실행 계획 파일 (exec-plan) 형식

`docs/exec-plans/active/` 에 작성. 파일명: `YYYYMMDD-작업명.md`

```markdown
# [작업명]

## 목표
한 문장으로 이 작업이 달성해야 할 것.

## 범위
- 수정/생성할 파일 목록
- 영향받는 레이어

## 완료 기준
- [ ] 빌드 통과
- [ ] 테스트 통과
- [ ] check_layers.py 통과
- [ ] 구체적인 기능 기준 1
- [ ] 구체적인 기능 기준 2

## 진행 상황
- [x] 완료된 항목
- [ ] 남은 항목

## 설계 결정 기록
작업 중 내린 결정과 이유를 여기에 기록.
(나중에 docs/design/으로 이동 가능)
```

작업 완료 시 `docs/exec-plans/completed/`로 이동.

---

## PR 작성 기준

PR을 열기 전 반드시:

```bash
# 1. 빌드 확인
cmake -B build && cmake --build build -j$(nproc)

# 2. 테스트 통과
cd build && ctest --output-on-failure

# 3. 레이어 위반 검사
python3 tools/check_layers.py

# 4. 포맷 적용
clang-format -i $(find src -name "*.cpp" -o -name "*.h")
```

PR 설명 필수 포함:
- **무엇을 했나**: 변경 내용 요약
- **왜 했나**: 동기, 설계 결정
- **영향 범위**: 수정된 레이어/도메인
- **테스트**: 어떻게 검증했나

---

## 에이전트가 막혔을 때

1. "더 열심히 시도"하지 않는다
2. 무엇이 빠진지 분석:
   - 문서가 부족한가? → `docs/`에 추가
   - 컨벤션이 불명확한가? → `docs/conventions/`에 추가
   - 린터가 없어 실수를 반복하는가? → `tools/`에 검사 추가
3. 부족한 것을 채운 후 다시 시도

---

## 금지 행동

- 레이어 의존성 위반 코드 커밋
- 테스트 없는 Core 레이어 신규 코드
- `check_layers.py` 통과 전 PR 생성
- exec-plan 없이 대형 작업 시작 (300줄 이상 변경 예상 시)
- 문서를 코드보다 나중에 작성 (설계 결정은 코딩 전 또는 동시에)
