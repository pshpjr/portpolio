#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ClientEquipmentWidgetBase.generated.h"

class UClientEquipmentComponent;

UCLASS(Abstract, BlueprintType, Blueprintable)
class CLIENT_API UClientEquipmentWidgetBase : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "UI")
    void BindEquipmentComponent(UClientEquipmentComponent* InEquipmentComponent);

    UFUNCTION(BlueprintPure, Category = "UI")
    UClientEquipmentComponent* GetEquipmentComponent() const;

protected:
    virtual void NativeDestruct() override;

    UFUNCTION(BlueprintImplementableEvent, Category = "UI")
    void RefreshFromEquipment();

private:
    UFUNCTION()
    void HandleEquipmentUpdated();

    UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UClientEquipmentComponent> EquipmentComponent = nullptr;
};
