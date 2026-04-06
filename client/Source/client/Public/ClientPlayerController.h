#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ClientPlayerController.generated.h"

class UUserWidget;

/**
 * 플레이어 컨트롤러.
 *
 * 담당:
 *   - 루트 HUD 위젯 생성/제거
 *   - 입력 모드 전환 (Game / UI / Game+UI)
 *   - UI 포커스 제어
 *   - 월드 상호작용 명령
 *   - 타겟팅 요청
 *   - 카메라/시점 (필요 시 하위 클래스에서 확장)
 *   - 현재 조작 관련 임시 상태
 *
 * 캐릭터 도메인 데이터(인벤토리, 장비 등)는 UClientCharacterSubsystem이 보유한다.
 * UI 상태(열린 화면, 드래그 등)는 UClientLocalPlayerSubsystem이 보유한다.
 */
UCLASS()
class CLIENT_API AClientPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    // ── HUD 위젯 관리 ────────────────────────────────────────────────────────

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowHUD();

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void HideHUD();

    UFUNCTION(BlueprintPure, Category = "HUD")
    UUserWidget* GetHUDWidget() const { return HUDWidget; }

    // ── 입력 모드 전환 ───────────────────────────────────────────────────────

    UFUNCTION(BlueprintCallable, Category = "Input")
    void SetGameInputMode();

    UFUNCTION(BlueprintCallable, Category = "Input")
    void SetUIOnlyInputMode();

    UFUNCTION(BlueprintCallable, Category = "Input")
    void SetGameAndUIInputMode();

    // ── UI 포커스 제어 ───────────────────────────────────────────────────────

    UFUNCTION(BlueprintCallable, Category = "Input")
    void FocusUI();

    UFUNCTION(BlueprintCallable, Category = "Input")
    void FocusGame();

    // ── 월드 상호작용 명령 ───────────────────────────────────────────────────

    UFUNCTION(BlueprintCallable, Category = "Interaction")
    void RequestInteraction(AActor* Target);

    // ── 타겟팅 요청 ──────────────────────────────────────────────────────────

    UFUNCTION(BlueprintCallable, Category = "Targeting")
    void RequestTargeting(AActor* Target);

    UFUNCTION(BlueprintCallable, Category = "Targeting")
    void ClearTarget();

    UFUNCTION(BlueprintPure, Category = "Targeting")
    AActor* GetCurrentTarget() const { return CurrentTarget.Get(); }

protected:
    /** Blueprint에서 사용할 HUD 위젯 클래스 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD")
    TSubclassOf<UUserWidget> HUDWidgetClass;

private:
    UPROPERTY()
    TObjectPtr<UUserWidget> HUDWidget;

    /** 현재 타겟팅 중인 액터 (임시 상태) */
    UPROPERTY()
    TWeakObjectPtr<AActor> CurrentTarget;
};
