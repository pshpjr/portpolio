#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ClientAnimInstance.generated.h"

/**
 * 플레이어 캐릭터 AnimInstance.
 *
 * 매 틱 캐릭터의 속도와 이동 방향(바라보는 방향 기준)을 계산해
 * Blend Space에서 사용할 변수로 노출한다.
 *
 * Blueprint AnimBP에서 이 클래스를 부모로 지정하고,
 * Speed / Direction 을 2D Blend Space에 연결한다.
 *
 * Direction 기준:
 *     0° = 바라보는 방향으로 전진
 *   +90° = 우측 스트레이프
 *  ±180° = 후진
 *   -90° = 좌측 스트레이프
 */
UCLASS()
class CLIENT_API UClientAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

    // ── Blend Space 입력 ────────────────────────────────────────────────────

    /** 지면 이동 속도 (cm/s). Idle ↔ Locomotion 전환 기준. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Locomotion")
    float Speed = 0.0f;

    /**
     * 바라보는 방향 기준 이동 각도 (-180 ~ 180).
     *  0 = 전진, +90 = 우측, -90 = 좌측, ±180 = 후진.
     */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Locomotion")
    float Direction = 0.0f;

    /** 공중에 있는지 여부. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Locomotion")
    bool bIsInAir = false;

    /** 이동 중인지 여부 (Speed > Threshold). */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Locomotion")
    bool bIsMoving = false;

    /** 이동 판정 임계값 (cm/s). */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Locomotion")
    float MovingThreshold = 3.0f;
};
