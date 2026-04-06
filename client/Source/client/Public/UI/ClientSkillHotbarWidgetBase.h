#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ClientSkillHotbarWidgetBase.generated.h"

class UClientSkillHotbarComponent;

UCLASS(Abstract, BlueprintType, Blueprintable)
class CLIENT_API UClientSkillHotbarWidgetBase : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "UI")
    void BindSkillHotbarComponent(UClientSkillHotbarComponent* InSkillHotbarComponent);

    UFUNCTION(BlueprintPure, Category = "UI")
    UClientSkillHotbarComponent* GetSkillHotbarComponent() const;

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    UFUNCTION(BlueprintImplementableEvent, Category = "UI")
    void RefreshFromSkillHotbar();

private:
    UFUNCTION()
    void HandleSkillHotbarUpdated();

    UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UClientSkillHotbarComponent> SkillHotbarComponent = nullptr;
};
