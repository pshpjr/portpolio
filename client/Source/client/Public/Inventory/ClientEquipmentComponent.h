#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/ClientInventoryTypes.h"
#include "ClientEquipmentComponent.generated.h"

class UClientItemInstance;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FClientEquipmentUpdatedSignature);

UCLASS(ClassGroup = (Custom), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class CLIENT_API UClientEquipmentComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UClientEquipmentComponent();

    UFUNCTION(BlueprintCallable, Category = "Equipment")
    void InitializeDefaultSlots();

    UFUNCTION(BlueprintPure, Category = "Equipment")
    bool CanEquipItem(const UClientItemInstance* Item, EClientEquipSlot Slot) const;

    UFUNCTION(BlueprintCallable, Category = "Equipment")
    bool EquipItem(UClientItemInstance* Item, EClientEquipSlot Slot);

    UFUNCTION(BlueprintCallable, Category = "Equipment")
    UClientItemInstance* UnequipItem(EClientEquipSlot Slot);

    UFUNCTION(BlueprintPure, Category = "Equipment")
    UClientItemInstance* GetEquippedItem(EClientEquipSlot Slot) const;

    UFUNCTION(BlueprintPure, Category = "Equipment")
    EClientWeaponType GetEquippedWeaponType() const;

    const TMap<EClientEquipSlot, TObjectPtr<UClientItemInstance>>& GetEquippedItems() const;

    UPROPERTY(BlueprintAssignable, Category = "Equipment")
    FClientEquipmentUpdatedSignature OnEquipmentUpdated;

private:
    void UpdateItemLocation(UClientItemInstance* Item, EClientEquipSlot Slot) const;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (AllowPrivateAccess = "true"))
    TMap<EClientEquipSlot, TObjectPtr<UClientItemInstance>> EquippedItems;
};
