#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/ClientInventoryTypes.h"
#include "ClientInventoryComponent.generated.h"

class UClientItemInstance;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FClientInventoryUpdatedSignature);

UCLASS(ClassGroup = (Custom), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class CLIENT_API UClientInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UClientInventoryComponent();

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void InitializeInventory(int32 InSlotCount = 32);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool AddItem(UClientItemInstance* Item, int32 PreferredSlotIndex = -1);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool RemoveItemByKey(const FString& ItemKey);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool MoveItem(int32 FromSlotIndex, int32 ToSlotIndex);

    UFUNCTION(BlueprintPure, Category = "Inventory")
    UClientItemInstance* GetItemAt(int32 SlotIndex) const;

    UFUNCTION(BlueprintPure, Category = "Inventory")
    int32 FindItemSlotByKey(const FString& ItemKey) const;

    const TArray<FClientInventorySlotState>& GetSlots() const;

    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FClientInventoryUpdatedSignature OnInventoryUpdated;

private:
    bool IsValidSlotIndex(int32 SlotIndex) const;
    bool TryStackIntoSlot(UClientItemInstance* SourceItem, int32 SlotIndex);
    void AssignItemToSlot(UClientItemInstance* Item, int32 SlotIndex);
    void ClearSlot(int32 SlotIndex);
    void BroadcastInventoryUpdated();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
    int32 SlotCount = 32;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
    TArray<FClientInventorySlotState> Slots;
};
