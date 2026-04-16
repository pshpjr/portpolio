#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ClientPlayerCharacter.generated.h"

/**
 * 플레이어 캐릭터.
 *
 * 담당:
 *   - 마우스 커서 방향으로 캐릭터 회전
 */
UCLASS()
class CLIENT_API AClientPlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AClientPlayerCharacter();

    virtual void Tick(float DeltaTime) override;

protected:
    /** true면 캐릭터가 매 틱 마우스 커서 방향을 바라본다. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mouse Look")
    bool bEnableMouseLook = true;

    /** 회전 보간 속도. 0이면 즉시 회전. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mouse Look",
              meta = (ClampMin = "0", EditCondition = "bEnableMouseLook"))
    float MouseLookInterpSpeed = 15.0f;

private:
    void UpdateMouseLookRotation(float DeltaTime);
};
