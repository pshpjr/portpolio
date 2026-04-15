#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ClientInventoryWidgetBase.generated.h"

class UClientInventoryComponent;
class UClientItemInstance;

UCLASS(Abstract, BlueprintType, Blueprintable)
class CLIENT_API UClientInventoryWidgetBase : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "UI")
    void BindInventoryComponent(UClientInventoryComponent* InInventoryComponent);

    UFUNCTION(BlueprintPure, Category = "UI")
    UClientInventoryComponent* GetInventoryComponent() const;

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    // 아이템이 추가·수량 변경되면 해당 아이템과 함께 호출된다.
    // ChangedItem이 null이면 아이템 제거 등으로 목록 순서가 바뀐 것이므로
    // GetInventoryComponent()->GetItems()로 전체 목록을 다시 읽어 렌더링한다.
    UFUNCTION(BlueprintImplementableEvent, Category = "UI")
    void OnInventoryUpdated(UClientItemInstance* ChangedItem);

private:
    UFUNCTION()
    void HandleInventoryUpdated(UClientItemInstance* ChangedItem);

    UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UClientInventoryComponent> InventoryComponent = nullptr;
};
