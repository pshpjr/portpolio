# 레이어 의존성 규칙

상세 규칙과 책임 분리는 [ARCHITECTURE.md](../../ARCHITECTURE.md)를 기준으로 한다.

## 규칙

```
Types → Config → Core → Service → Network → Runtime
```

- 각 레이어는 자신의 왼쪽 레이어만 의존한다.
- 같은 레이어 내부 순환 의존은 허용하지 않는다.
- 위반 검사는 `python ../tools/check_layers.py --root src --layer-order types,config,core,service,network,runtime`로 수행한다.
