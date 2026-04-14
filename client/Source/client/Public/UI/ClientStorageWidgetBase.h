#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Layout/Margin.h"
#include "ClientStorageWidgetBase.generated.h"

class UClientStorageComponent;
class UInventorySlot;
class UUniformGridPanel;

UCLASS(Abstract, BlueprintType, Blueprintable)
class CLIENT_API UClientStorageWidgetBase : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "UI")
    void BindStorageComponent(UClientStorageComponent* InStorageComponent);

    UFUNCTION(BlueprintPure, Category = "UI")
    UClientStorageComponent* GetStorageComponent() const;

    /** 현재 그릴 탭 인덱스. BP에서 탭 버튼으로 변경 후 Refresh 호출. */
    UFUNCTION(BlueprintCallable, Category = "UI")
    void SetActiveTabIndex(int32 NewTabIndex);

    UFUNCTION(BlueprintPure, Category = "UI")
    int32 GetActiveTabIndex() const { return ActiveTabIndex; }

    /** 슬롯 위젯 클래스. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout")
    TSubclassOf<UInventorySlot> SlotClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout", meta = (ClampMin = "1"))
    int32 NumColumns = 8;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout")
    FMargin SlotPadding = FMargin(2.f);

    /** 각 슬롯의 고정 크기(px). 0 이하면 override 미적용. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout")
    FVector2D SlotSize = FVector2D(64.f, 64.f);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual void NativePreConstruct() override;

    /** 선택적 훅: 기본 갱신 뒤 BP 후처리. */
    UFUNCTION(BlueprintImplementableEvent, Category = "UI")
    void RefreshFromStorage();

    void RebuildSlots();
    void RefreshSlots();

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UUniformGridPanel> SlotContainer = nullptr;

private:
    UFUNCTION()
    void HandleStorageUpdated();

    UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UClientStorageComponent> StorageComponent = nullptr;

    UPROPERTY(Transient)
    TArray<TObjectPtr<UInventorySlot>> SlotWidgets;

    UPROPERTY(EditAnywhere, Category = "UI", meta = (AllowPrivateAccess = "true", ClampMin = "0"))
    int32 ActiveTabIndex = 0;
};
