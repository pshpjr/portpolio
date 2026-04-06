#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ClientConsumableQuickSlotWidgetBase.generated.h"

class UClientConsumableQuickSlotComponent;

UCLASS(Abstract, BlueprintType, Blueprintable)
class CLIENT_API UClientConsumableQuickSlotWidgetBase : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "UI")
    void BindConsumableQuickSlotComponent(UClientConsumableQuickSlotComponent* InComponent);

    UFUNCTION(BlueprintPure, Category = "UI")
    UClientConsumableQuickSlotComponent* GetConsumableQuickSlotComponent() const;

protected:
    virtual void NativeDestruct() override;

    UFUNCTION(BlueprintImplementableEvent, Category = "UI")
    void RefreshFromConsumableQuickSlot();

private:
    UFUNCTION()
    void HandleConsumableQuickSlotUpdated();

    UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UClientConsumableQuickSlotComponent> ConsumableQuickSlotComponent = nullptr;
};
