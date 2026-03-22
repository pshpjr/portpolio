# 세션 종료 시 처리 — OnCloseSession 이후 OnPacket 미호출 보장 논의 기록

## 목표

OnCloseSession 호출 이후 해당 세션에 대해 OnPacket이 절대 호출되지 않음을 보장한다.

---

## 전제 조건

라이브러리 레벨에서 **serial recv**(한 번에 하나의 async_read만 pending)를 보장한다. 이에 따라 recv 완료 핸들러가 "다음 recv를 걸지 말지" 결정하는 유일한 제어 지점이 된다.

OnPacket 콜백이 IO 스레드에서 실행되는지 별도 로직 스레드로 넘기는지는 라이브러리가 정할 사항이 아니다. 라이브러리가 serial recv를 보장하면, 컨텐츠 레이어에서 어떤 스레딩 모델을 쓰든 순서 보장이 가능하다.

---

## 기본 흐름 설계

```
void Session::OnRecvComplete(error_code ec) {
    if (ec || closed_) {
        onCloseSession_(shared_from_this());
        return;
    }

    onPacket_(shared_from_this(), parsed_packet);

    if (!closed_) {
        PostNextRecv();
    } else {
        onCloseSession_(shared_from_this());
    }
}
```

serial recv이므로 OnPacket과 다음 recv 사이에 `closed_` 체크 기회가 항상 존재한다. 이 체크를 통과하지 못하면 recv를 걸지 않고 OnCloseSession을 호출한다.

---

## OnCloseSession이 발생하는 두 경로

### 경로 A — 네트워크 측 감지

상대방 연결 종료 또는 read 에러 → recv 완료 핸들러에서 에러 확인 → OnCloseSession 호출. 비교적 단순하다.

### 경로 B — 컨텐츠 측 요청

컨텐츠가 `Session::Close()` 호출 → 어떤 메커니즘으로든 OnCloseSession 호출. Close()가 외부 스레드에서도 호출 가능하므로, recv 완료 핸들러와의 경쟁 조건이 핵심 문제이다.

---

## 외부 Close()와 recv 핸들러의 경쟁 해결 — Atomic Counter 패턴

### 메커니즘

Session에 atomic counter를 두어 현재 걸려 있는 IO 횟수를 추적한다.

- 값 2: send + recv 진행 중
- 값 1: recv만 진행 중
- Close() 호출 시 counter를 1 차감
- send/recv 완료 핸들러가 처리를 끝낸 뒤 counter를 차감
- **counter를 0 미만(음수)으로 만든 스레드가 OnDisconnect를 호출**한다

### 핵심 성질: recv 핸들러 처리 중에는 음수 불가

recv 핸들러가 자기 카운트를 유지하고 있으므로, 핸들러 실행 중에 counter가 음수가 될 수 없다. 외부에서 Close()를 호출해 차감하더라도, recv 핸들러의 몫이 남아 있기 때문이다.

### Close() 호출 시 시나리오

**recv pending 중 외부 Close():**

```
[IO 스레드]                        [외부 스레드]
recv pending (counter=1)            Close() → counter 1→0, socket close
recv 에러로 완료
에러이므로 OnPacket 없이 차감 → 0→-1
→ 이 스레드가 음수를 만들었으므로 OnDisconnect 호출
```

OnPacket 호출 없이 OnDisconnect가 호출된다. 보장 성립.

**OnPacket 처리 중 외부 Close():**

```
[IO 스레드]                        [외부 스레드]
핸들러 진입 (counter 아직 1)
OnPacket 호출 중...                Close() → counter 1→0, socket close
OnPacket 리턴
close 플래그 확인 → true
recv 안 걸고 close 함수 호출 → 0→-1
→ 이 스레드가 음수를 만들었으므로 OnDisconnect 호출
```

recv 핸들러가 OnPacket 리턴 후 close 플래그를 확인하고, 자체적으로 close 함수를 호출하여 counter를 차감한다. OnPacket이 반드시 끝난 뒤에 OnDisconnect가 온다. 보장 성립.

### Send 중 Close의 처리

send close의 경우 이 카운터 루프 외부에서 별도 처리한다. 이 경우 OnCloseSession은 컨텐츠 레이어에서 호출할 수 있다.

---

## 미해결: OnDisconnect 호출 스레드 문제

atomic counter 패턴에서 counter를 음수로 만드는 스레드가 누구냐에 따라 OnDisconnect가 **외부 스레드(컨텐츠 스레드)**에서 호출될 수 있다. 이를 해결하기 위한 방안으로 두 가지가 논의되었다.

### 방법 1 — Close() 자체를 io_context로 post

```cpp
void Session::Close() {
    io_context_.post([self = shared_from_this()] {
        self->DoClose();
    });
}
```

DoClose가 항상 IO 스레드에서 실행되므로, counter를 음수로 만드는 것도 IO 스레드이다. OnDisconnect가 컨텐츠 스레드에서 호출될 일이 없다. 사실상 Close()에 대해서만 strand를 도입하는 것과 동일하다.

단점: Close() 호출 시점에 소켓이 즉시 닫히지 않을 수 있다(post 큐 지연).

### 방법 2 — Close()는 즉시 실행, OnDisconnect만 post

소켓은 즉시 닫히고 콜백만 IO 스레드에서 실행하는 방식이나, `socket_.close()`를 외부 스레드에서 호출하면 recv 완료 핸들러와 소켓에 대한 경합이 발생한다. Boost.Asio의 socket은 thread-safe하지 않으므로, `socket_.close()` 자체도 결국 post해야 안전하다. 이 경우 방법 1과 동일해진다.

### 결론

**방법 1(Close() 전체를 post)이 자연스럽다.** 전체 strand를 도입하지 않으면서도, 컨텐츠 스레드에서 OnDisconnect 호출되는 문제만 깔끔하게 해결된다.

---

## strand와의 비교

### strand 내부 구현

핵심은 소유권 패턴이다. `running_` 플래그를 true로 만든 스레드가 큐를 독점적으로 drain한다. 다른 스레드는 큐에 넣기만 하고 돌아간다. 같은 strand에 post된 핸들러들은 절대 동시 실행되지 않는다.

### 비교

- **공통점**: 둘 다 "누가 정리 책임을 지는가"를 atomic 연산으로 결정한다.
- **범위**: atomic counter는 recv/send/close 각각에서 카운터로 판단. strand는 세션의 모든 이벤트를 하나의 직렬 큐로 통합.
- **확장성**: atomic counter는 새 이벤트 종류 추가 시 카운터 규칙 재검증 필요. strand는 자연스럽게 대응.
- **성능**: atomic counter는 `fetch_sub` 한 번으로 가벼움. strand는 매 핸들러마다 mutex lock/unlock + queue push/pop.
- **판단 기준**: 이벤트 종류가 recv/send/close로 고정이면 atomic counter가 유리. 타이머 등 비동기 연산이 세션에 추가될 가능성이 있으면 strand가 유리.

---

## 최종 미해결 사항

1. **Close()를 post하는 방식의 최종 확정** — 방법 1(Close 전체 post)로 갈 것인지.
2. **strand 전면 도입 여부** — atomic counter + Close post 혼합으로 갈 것인지, 전체 strand로 갈 것인지. 향후 세션에 붙을 이벤트 종류(타이머 등)에 따라 결정.
3. **이전 문서의 미해결 사항 7-2(OnCloseSession 내부 파괴 순서)** — OnDisconnect 호출 스레드가 확정되면 이어서 논의.
