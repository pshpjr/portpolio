#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ClientInventoryWidgetBase.generated.h"

class UClientInventoryComponent;

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
    virtual void NativeDestruct() override;

    UFUNCTION(BlueprintImplementableEvent, Category = "UI")
    void RefreshFromInventory();

private:
    UFUNCTION()
    void HandleInventoryUpdated();

    UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UClientInventoryComponent> InventoryComponent = nullptr;
};
