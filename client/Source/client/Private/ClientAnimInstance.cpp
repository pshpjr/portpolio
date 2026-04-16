#include "ClientAnimInstance.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "KismetAnimationLibrary.h"

void UClientAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();
}

void UClientAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    APawn* Pawn = TryGetPawnOwner();
    if (Pawn == nullptr)
    {
        return;
    }

    // ── 속도 ────────────────────────────────────────────────────────────────
    const FVector Velocity = Pawn->GetVelocity();
    Speed = Velocity.Size2D();
    bIsMoving = Speed > MovingThreshold;

    // ── 방향 (바라보는 방향 기준 이동 각도) ─────────────────────────────────
    if (bIsMoving)
    {
        const FRotator ActorRotation = Pawn->GetActorRotation();
        Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, ActorRotation);
    }
    else
    {
        Direction = 0.0f;
    }

    // ── 공중 여부 ───────────────────────────────────────────────────────────
    if (const ACharacter* Character = Cast<ACharacter>(Pawn))
    {
        if (const UCharacterMovementComponent* CMC = Character->GetCharacterMovement())
        {
            bIsInAir = CMC->IsFalling();
        }
    }
}
