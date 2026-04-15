#include "ClientPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "UI/ClientMainHUD.h"

void AClientPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (ULocalPlayer* LP = GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* Sub =
                LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            if (DefaultMappingContext != nullptr)
            {
                Sub->AddMappingContext(DefaultMappingContext, DefaultMappingPriority);
            }
        }
    }
}

void AClientPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
    if (EIC == nullptr)
    {
        return;
    }

    for (const FClientHUDPanelToggleBinding& Binding : PanelToggleBindings)
    {
        if (Binding.Action == nullptr || Binding.PanelId.IsNone())
        {
            continue;
        }
        EIC->BindAction(
            Binding.Action,
            ETriggerEvent::Triggered,
            this,
            &AClientPlayerController::OnTogglePanelAction,
            Binding.PanelId);
    }

    if (CloseTopAction != nullptr)
    {
        EIC->BindAction(
            CloseTopAction,
            ETriggerEvent::Triggered,
            this,
            &AClientPlayerController::OnCloseTopAction);
    }
}

UClientMainHUD* AClientPlayerController::GetMainHUD() const
{
    return Cast<UClientMainHUD>(HUDWidget);
}

void AClientPlayerController::TogglePanel(FName PanelId)
{
    if (UClientMainHUD* HUD = GetMainHUD())
    {
        HUD->TogglePanel(PanelId);
    }
}

void AClientPlayerController::OpenPanel(FName PanelId)
{
    if (UClientMainHUD* HUD = GetMainHUD())
    {
        HUD->OpenPanel(PanelId);
    }
}

void AClientPlayerController::ClosePanel(FName PanelId)
{
    if (UClientMainHUD* HUD = GetMainHUD())
    {
        HUD->ClosePanel(PanelId);
    }
}

bool AClientPlayerController::CloseTopmostPanel()
{
    if (UClientMainHUD* HUD = GetMainHUD())
    {
        return HUD->CloseTopmost();
    }
    return false;
}

void AClientPlayerController::SyncInputModeForHUD()
{
    UClientMainHUD* HUD = GetMainHUD();
    const bool bNeedCursor = HUD != nullptr && HUD->AnyOpenPanelNeedsCursor();

    if (bNeedCursor)
    {
        SetGameAndUIInputMode();
    }
    else
    {
        SetGameInputMode();
    }
}

void AClientPlayerController::OnTogglePanelAction(const FInputActionValue& /*Value*/, FName PanelId)
{
    TogglePanel(PanelId);
}

void AClientPlayerController::OnCloseTopAction(const FInputActionValue& /*Value*/)
{
    CloseTopmostPanel();
}

void AClientPlayerController::HandleHUDPanelVisibilityChanged(FName /*PanelId*/, bool /*bOpen*/)
{
    SyncInputModeForHUD();
}

void AClientPlayerController::ShowHUD()
{
    if (HUDWidget == nullptr && HUDWidgetClass != nullptr)
    {
        HUDWidget = CreateWidget<UUserWidget>(this, HUDWidgetClass);
        if (UClientMainHUD* HUD = Cast<UClientMainHUD>(HUDWidget))
        {
            HUD->OnPanelVisibilityChanged.AddDynamic(
                this, &AClientPlayerController::HandleHUDPanelVisibilityChanged);
        }
    }

    if (HUDWidget != nullptr)
    {
        HUDWidget->AddToViewport();
    }

    SyncInputModeForHUD();
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
    //SetFocus();
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
