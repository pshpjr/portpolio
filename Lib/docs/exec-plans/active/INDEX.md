# Active Exec Plans
> `Lib/` 영역의 활성 실행 계획 인덱스입니다.
> 새 세션에서는 이 파일만 먼저 읽고, 관련 있는 plan 본문만 엽니다.

## 현재 활성 계획

- [20260328-strongly-typed-id-helper.md](./20260328-strongly-typed-id-helper.md) - 콘텐츠 아이템 ID, 스킬 ID 등에 쓸 공용 강타입 식별자 헬퍼 추가
- [20260328-connect-lib-to-server.md](./20260328-connect-lib-to-server.md) - `Lib` 타깃을 정리해 `server`가 공용 헤더를 소비할 수 있게 연결
- [20260317-utility-functions.md](./20260317-utility-functions.md) - 문자열·컨테이너·랜덤 유틸리티 추가
- [20260317-assert-utility.md](./20260317-assert-utility.md) - Assert 런타임 불변식 검증 유틸리티 구현

## 운영 규칙

- 현재 작업과 직접 관련 없는 active plan은 읽지 않는다.
- Lib의 모든 작업은 크기와 무관하게 exec-plan 작성으로 시작한다.
- exec-plan 또는 사용자 검토 없이 코드, 빌드 설정, 테스트를 수정하면 안 된다.
- 완료된 plan은 `completed/`로 이동한다.
