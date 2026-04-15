#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Inventory/ClientInventoryTypes.h"
#include "ClientEquipmentComponent.generated.h"

class UClientItemInstance;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FClientEquipmentUpdatedSignature);

UCLASS(BlueprintType)
class CLIENT_API UClientEquipmentComponent : public UObject
{
    GENERATED_BODY()

public:
    UClientEquipmentComponent();

    UFUNCTION(BlueprintCallable, Category = "Equipment")
    void InitializeDefaultSlots();

    UFUNCTION(BlueprintPure, Category = "Equipment")
    bool CanEquipItem(const UClientItemInstance* Item, EEquipSlot Slot) const;

    // 슬롯이 비어있으면 장착, 이미 장착 중이면 기존 아이템을 반환하고 교체한다.
    UFUNCTION(BlueprintCallable, Category = "Equipment")
    bool EquipItem(UClientItemInstance* Item, EEquipSlot Slot);

    // 교체 장착: 기존 장착 아이템을 OutPreviousItem으로 반환하고 새 아이템을 장착한다.
    UFUNCTION(BlueprintCallable, Category = "Equipment")
    bool SwapEquipItem(UClientItemInstance* Item, EEquipSlot Slot, UClientItemInstance*& OutPreviousItem);

    UFUNCTION(BlueprintCallable, Category = "Equipment")
    UClientItemInstance* UnequipItem(EEquipSlot Slot);

    UFUNCTION(BlueprintPure, Category = "Equipment")
    UClientItemInstance* GetEquippedItem(EEquipSlot Slot) const;

    UFUNCTION(BlueprintPure, Category = "Equipment")
    EWeaponType GetEquippedWeaponType() const;

    const TMap<EEquipSlot, TObjectPtr<UClientItemInstance>>& GetEquippedItems() const;

    UPROPERTY(BlueprintAssignable, Category = "Equipment")
    FClientEquipmentUpdatedSignature OnEquipmentUpdated;

private:
    void UpdateItemLocation(UClientItemInstance* Item, EEquipSlot Slot) const;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (AllowPrivateAccess = "true"))
    TMap<EEquipSlot, TObjectPtr<UClientItemInstance>> EquippedItems;
};
