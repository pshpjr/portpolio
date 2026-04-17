#include "ClientPlayerCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

AClientPlayerCharacter::AClientPlayerCharacter()
{
    bUseControllerRotationYaw = false;

    if (UCharacterMovementComponent* CMC = GetCharacterMovement())
    {
        CMC->bOrientRotationToMovement = false;
    }
}

void AClientPlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bEnableMouseLook)
    {
        UpdateMouseLookRotation(DeltaTime);
    }
}

void AClientPlayerCharacter::UpdateMouseLookRotation(float DeltaTime)
{
    APlayerController* PC = Cast<APlayerController>(GetController());
    if (PC == nullptr)
    {
        return;
    }

    FHitResult Hit;
    if (!PC->GetHitResultUnderCursor(ECC_Visibility, false, Hit))
    {
        return;
    }

    const FVector PawnLocation = GetActorLocation();
    const FVector Target       = Hit.ImpactPoint;
    const FVector Direction    = Target - PawnLocation;

    if (Direction.SizeSquared2D() < 1.0f)
    {
        return;
    }

    const FRotator TargetRotation = FRotator(0.0f, Direction.Rotation().Yaw, 0.0f);

    if (MouseLookInterpSpeed <= 0.0f)
    {
        SetActorRotation(TargetRotation);
    }
    else
    {
        const FRotator Current      = GetActorRotation();
        const FRotator Interpolated = FMath::RInterpTo(Current, TargetRotation, DeltaTime, MouseLookInterpSpeed);
        SetActorRotation(Interpolated);
    }
}
