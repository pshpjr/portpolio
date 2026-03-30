#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/ClientInventoryTypes.h"
#include "ClientSkillHotbarComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FClientSkillHotbarUpdatedSignature);

UCLASS(ClassGroup = (Custom), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class CLIENT_API UClientSkillHotbarComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UClientSkillHotbarComponent();

    UFUNCTION(BlueprintCallable, Category = "Skill")
    void SetEquippedWeaponType(EClientWeaponType InWeaponType);

    UFUNCTION(BlueprintPure, Category = "Skill")
    bool CanRegisterSkillToSlot(const FClientSkillDefinition& SkillDefinition, EClientSkillHotSlot Slot) const;

    UFUNCTION(BlueprintCallable, Category = "Skill")
    bool RegisterSkillToSlot(const FClientSkillDefinition& SkillDefinition, EClientSkillHotSlot Slot);

    UFUNCTION(BlueprintCallable, Category = "Skill")
    bool ClearSlot(EClientSkillHotSlot Slot);

    UFUNCTION(BlueprintPure, Category = "Skill")
    bool TryGetSkillAtSlot(EClientSkillHotSlot Slot, FClientSkillDefinition& OutSkillDefinition) const;

    UFUNCTION(BlueprintPure, Category = "Skill")
    EClientSkillHotSlot FindSlotBySkillKey(const FString& SkillKey) const;

    const TMap<EClientSkillHotSlot, FClientSkillDefinition>& GetSlotBindings() const;

    UPROPERTY(BlueprintAssignable, Category = "Skill")
    FClientSkillHotbarUpdatedSignature OnSkillHotbarUpdated;

private:
    bool IsActiveSkillSlot(EClientSkillHotSlot Slot) const;
    void BroadcastHotbarUpdated();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill", meta = (AllowPrivateAccess = "true"))
    EClientWeaponType EquippedWeaponType = EClientWeaponType::None;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill", meta = (AllowPrivateAccess = "true"))
    TMap<EClientSkillHotSlot, FClientSkillDefinition> SlotBindings;
};
