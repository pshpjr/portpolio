# Dependencies
> 외부 라이브러리와 사용 레이어를 정리한 참조 문서.

| 라이브러리 | 용도 | 사용 레이어 |
|---|---|---|
| `spdlog` | 로깅 | Runtime, Network |
| `FlatBuffers` | 직렬화 | Config, Network |
| `Boost.Asio` | 비동기 네트워크 / executor | Network |
| `nlohmann/json` | 설정 파싱 | Config |
| `GoogleTest` | 테스트 | tests 전용 |
