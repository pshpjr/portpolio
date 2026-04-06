#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "ClientLocalPlayerSubsystem.generated.h"

/** UI 레이어 우선순위 */
UENUM(BlueprintType)
enum class EClientUILayer : uint8
{
    None     UMETA(DisplayName = "None"),
    HUD      UMETA(DisplayName = "HUD"),
    Menu     UMETA(DisplayName = "Menu"),
    Popup    UMETA(DisplayName = "Popup"),
    Toast    UMETA(DisplayName = "Toast"),
};

/** 드래그 중인 슬롯 정보 */
USTRUCT(BlueprintType)
struct FClientDragState
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    bool bIsDragging = false;

    /** 드래그 출처 컨텍스트 (예: "Inventory", "Equipment") */
    UPROPERTY(BlueprintReadWrite)
    FName SourceContext;

    UPROPERTY(BlueprintReadWrite)
    int32 SourceSlotIndex = INDEX_NONE;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FClientScreenOpenedSignature, FName, ScreenName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FClientScreenClosedSignature, FName, ScreenName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FClientNotificationPushedSignature, const FText&, Message);

/**
 * LocalPlayerSubsystem — HUD/UI 상태 모델.
 *
 * 어떤 화면이 열렸는지, 드래그 상태, 로컬 선택 상태,
 * 타겟 프레임 표시용 상태, 알림/팝업 큐, UI 레이어 상태를 관리한다.
 *
 * 접근 방법:
 *   GetOwningLocalPlayer()->GetSubsystem<UClientLocalPlayerSubsystem>()
 */
UCLASS()
class CLIENT_API UClientLocalPlayerSubsystem : public ULocalPlayerSubsystem
{
    GENERATED_BODY()

public:
    // ── 화면 열림/닫힘 ───────────────────────────────────────────────────────

    UFUNCTION(BlueprintCallable, Category = "UI|Screen")
    void OpenScreen(FName ScreenName);

    UFUNCTION(BlueprintCallable, Category = "UI|Screen")
    void CloseScreen(FName ScreenName);

    UFUNCTION(BlueprintPure, Category = "UI|Screen")
    bool IsScreenOpen(FName ScreenName) const;

    UFUNCTION(BlueprintPure, Category = "UI|Screen")
    const TArray<FName>& GetOpenScreens() const { return OpenScreens; }

    UPROPERTY(BlueprintAssignable, Category = "UI|Screen")
    FClientScreenOpenedSignature OnScreenOpened;

    UPROPERTY(BlueprintAssignable, Category = "UI|Screen")
    FClientScreenClosedSignature OnScreenClosed;

    // ── 드래그 상태 ──────────────────────────────────────────────────────────

    UFUNCTION(BlueprintCallable, Category = "UI|Drag")
    void BeginDrag(FName SourceContext, int32 SourceSlotIndex);

    UFUNCTION(BlueprintCallable, Category = "UI|Drag")
    void EndDrag();

    UFUNCTION(BlueprintPure, Category = "UI|Drag")
    const FClientDragState& GetDragState() const { return DragState; }

    // ── 로컬 선택 상태 ───────────────────────────────────────────────────────

    UFUNCTION(BlueprintCallable, Category = "UI|Selection")
    void SetLocalSelection(FName Context, int32 SlotIndex);

    UFUNCTION(BlueprintCallable, Category = "UI|Selection")
    void ClearLocalSelection();

    UFUNCTION(BlueprintPure, Category = "UI|Selection")
    bool HasLocalSelection() const { return SelectedSlotIndex != INDEX_NONE; }

    UFUNCTION(BlueprintPure, Category = "UI|Selection")
    int32 GetSelectedSlotIndex() const { return SelectedSlotIndex; }

    UFUNCTION(BlueprintPure, Category = "UI|Selection")
    FName GetSelectedContext() const { return SelectedContext; }

    // ── 타겟 프레임 표시용 상태 ──────────────────────────────────────────────

    UFUNCTION(BlueprintCallable, Category = "UI|Target")
    void SetTargetActor(AActor* InTarget);

    UFUNCTION(BlueprintPure, Category = "UI|Target")
    AActor* GetTargetActor() const { return TargetActor.Get(); }

    // ── 알림/팝업 큐 ─────────────────────────────────────────────────────────

    UFUNCTION(BlueprintCallable, Category = "UI|Notification")
    void PushNotification(const FText& Message);

    UPROPERTY(BlueprintAssignable, Category = "UI|Notification")
    FClientNotificationPushedSignature OnNotificationPushed;

    // ── UI 내비게이션/레이어 상태 ─────────────────────────────────────────────

    UFUNCTION(BlueprintPure, Category = "UI|Layer")
    EClientUILayer GetActiveLayer() const { return ActiveLayer; }

    UFUNCTION(BlueprintCallable, Category = "UI|Layer")
    void SetActiveLayer(EClientUILayer Layer);

private:
    UPROPERTY()
    TArray<FName> OpenScreens;

    UPROPERTY()
    FClientDragState DragState;

    UPROPERTY()
    FName SelectedContext;

    int32 SelectedSlotIndex = INDEX_NONE;

    UPROPERTY()
    TWeakObjectPtr<AActor> TargetActor;

    UPROPERTY()
    EClientUILayer ActiveLayer = EClientUILayer::None;
};
