#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ClientStorageWidgetBase.generated.h"

class UClientStorageComponent;

UCLASS(Abstract, BlueprintType, Blueprintable)
class CLIENT_API UClientStorageWidgetBase : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "UI")
    void BindStorageComponent(UClientStorageComponent* InStorageComponent);

    UFUNCTION(BlueprintPure, Category = "UI")
    UClientStorageComponent* GetStorageComponent() const;

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    UFUNCTION(BlueprintImplementableEvent, Category = "UI")
    void RefreshFromStorage();

private:
    UFUNCTION()
    void HandleStorageUpdated();

    UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UClientStorageComponent> StorageComponent = nullptr;
};
