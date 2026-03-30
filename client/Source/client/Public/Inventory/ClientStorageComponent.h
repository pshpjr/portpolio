#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/ClientInventoryTypes.h"
#include "ClientStorageComponent.generated.h"

class UClientItemInstance;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FClientStorageUpdatedSignature);

UCLASS(ClassGroup = (Custom), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class CLIENT_API UClientStorageComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UClientStorageComponent();

    UFUNCTION(BlueprintCallable, Category = "Storage")
    void InitializeStorage(const TArray<FClientStorageTabDescriptor>& TabDescriptors);

    UFUNCTION(BlueprintCallable, Category = "Storage")
    void InitializeDefaultSharedStorage(int32 TabCount = 1, int32 SlotsPerTab = 48);

    UFUNCTION(BlueprintCallable, Category = "Storage")
    bool AddItemToTab(UClientItemInstance* Item, int32 TabIndex, int32 PreferredSlotIndex = INDEX_NONE);

    UFUNCTION(BlueprintCallable, Category = "Storage")
    bool MoveItem(int32 FromTabIndex, int32 FromSlotIndex, int32 ToTabIndex, int32 ToSlotIndex);

    UFUNCTION(BlueprintCallable, Category = "Storage")
    UClientItemInstance* RemoveItem(int32 TabIndex, int32 SlotIndex);

    UFUNCTION(BlueprintPure, Category = "Storage")
    UClientItemInstance* GetItemAt(int32 TabIndex, int32 SlotIndex) const;

    const TArray<FClientStorageTabState>& GetTabs() const;

    UPROPERTY(BlueprintAssignable, Category = "Storage")
    FClientStorageUpdatedSignature OnStorageUpdated;

private:
    bool IsValidTabIndex(int32 TabIndex) const;
    bool IsValidSlotIndex(int32 TabIndex, int32 SlotIndex) const;
    bool TryStackIntoSlot(UClientItemInstance* SourceItem, int32 TabIndex, int32 SlotIndex);
    void AssignItemToSlot(UClientItemInstance* Item, int32 TabIndex, int32 SlotIndex);
    void ClearSlot(int32 TabIndex, int32 SlotIndex);
    void BroadcastStorageUpdated();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Storage", meta = (AllowPrivateAccess = "true"))
    TArray<FClientStorageTabState> Tabs;
};
