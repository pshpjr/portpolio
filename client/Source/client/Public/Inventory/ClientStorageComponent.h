#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Inventory/ClientInventoryTypes.h"
#include "ClientStorageComponent.generated.h"

class UClientItemInstance;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FClientStorageUpdatedSignature);

UCLASS(BlueprintType)
class CLIENT_API UClientStorageComponent : public UObject
{
    GENERATED_BODY()

public:
    UClientStorageComponent();

    UFUNCTION(BlueprintCallable, Category = "Storage")
    void InitializeStorage(const TArray<FClientStorageTabDescriptor>& TabDescriptors);

    // 기본값 24는 GClientStorageTabDefaultCapacity와 동일. UHT가 constexpr 기본 인자를 파싱 못 해 리터럴로 둠.
    UFUNCTION(BlueprintCallable, Category = "Storage")
    void InitializeDefaultSharedStorage(int32 TabCount = 1, int32 SlotsPerTab = 24);

    /** 서버가 준 현재 용량으로 탭 Capacity 갱신. [0, 탭 MaxCapacity]로 클램프, OnStorageUpdated 브로드캐스트. */
    UFUNCTION(BlueprintCallable, Category = "Storage")
    void SetTabCurrentCapacity(int32 TabIndex, int32 NewCapacity);

    UFUNCTION(BlueprintPure, Category = "Storage")
    int32 GetTabCurrentCapacity(int32 TabIndex) const;

    UFUNCTION(BlueprintPure, Category = "Storage")
    int32 GetTabMaxCapacity(int32 TabIndex) const;

    UFUNCTION(BlueprintCallable, Category = "Storage")
    bool AddItemToTab(UClientItemInstance* Item, int32 TabIndex, int32 PreferredSlotIndex = -1);

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
