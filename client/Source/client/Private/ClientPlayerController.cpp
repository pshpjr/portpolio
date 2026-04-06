#include "ClientPlayerController.h"

#include "Blueprint/UserWidget.h"

void AClientPlayerController::ShowHUD()
{
    if (HUDWidget == nullptr && HUDWidgetClass != nullptr)
    {
        HUDWidget = CreateWidget<UUserWidget>(this, HUDWidgetClass);
    }

    if (HUDWidget != nullptr)
    {
        HUDWidget->AddToViewport();
    }
}

void AClientPlayerController::HideHUD()
{
    if (HUDWidget != nullptr)
    {
        HUDWidget->RemoveFromParent();
    }
}

void AClientPlayerController::SetGameInputMode()
{
    FInputModeGameOnly InputMode;
    SetInputMode(InputMode);
    SetShowMouseCursor(false);
}

void AClientPlayerController::SetUIOnlyInputMode()
{
    FInputModeUIOnly InputMode;
    SetInputMode(InputMode);
    SetShowMouseCursor(true);
}

void AClientPlayerController::SetGameAndUIInputMode()
{
    FInputModeGameAndUI InputMode;
    SetInputMode(InputMode);
    SetShowMouseCursor(true);
}

void AClientPlayerController::FocusUI()
{
    if (HUDWidget != nullptr)
    {
        HUDWidget->SetFocus();
    }
}

void AClientPlayerController::FocusGame()
{
    SetFocus();
}

void AClientPlayerController::RequestInteraction(AActor* Target)
{
    // 구현 예정: 서버에 상호작용 패킷 전송
}

void AClientPlayerController::RequestTargeting(AActor* Target)
{
    CurrentTarget = Target;
}

void AClientPlayerController::ClearTarget()
{
    CurrentTarget = nullptr;
}
