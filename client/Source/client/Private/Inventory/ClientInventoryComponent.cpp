#include "Inventory/ClientInventoryComponent.h"

#include "Inventory/ClientItemInstance.h"

UClientInventoryComponent::UClientInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    InitializeInventory(SlotCount);
}

void UClientInventoryComponent::InitializeInventory(int32 InSlotCount)
{
    SlotCount = FMath::Max(1, InSlotCount);
    Slots.SetNum(SlotCount);
}

bool UClientInventoryComponent::AddItem(UClientItemInstance* Item, int32 PreferredSlotIndex)
{
    if (Item == nullptr)
    {
        return false;
    }

    if (!Item->CanMoveToStorage(EClientItemStorageKind::Inventory))
    {
        return false;
    }

    if (Item->IsStackable())
    {
        for (int32 SlotIndex = 0; SlotIndex < Slots.Num(); ++SlotIndex)
        {
            if (TryStackIntoSlot(Item, SlotIndex) && Item->GetRuntimeData().Count == 0)
            {
                BroadcastInventoryUpdated();
                return true;
            }
        }
    }

    if (IsValidSlotIndex(PreferredSlotIndex) && Slots[PreferredSlotIndex].Item == nullptr)
    {
        AssignItemToSlot(Item, PreferredSlotIndex);
        BroadcastInventoryUpdated();
        return true;
    }

    for (int32 SlotIndex = 0; SlotIndex < Slots.Num(); ++SlotIndex)
    {
        if (Slots[SlotIndex].Item == nullptr)
        {
            AssignItemToSlot(Item, SlotIndex);
            BroadcastInventoryUpdated();
            return true;
        }
    }

    return false;
}

bool UClientInventoryComponent::RemoveItemByKey(const FString& ItemKey)
{
    const int32 SlotIndex = FindItemSlotByKey(ItemKey);
    if (!IsValidSlotIndex(SlotIndex))
    {
        return false;
    }

    ClearSlot(SlotIndex);
    BroadcastInventoryUpdated();
    return true;
}

bool UClientInventoryComponent::MoveItem(int32 FromSlotIndex, int32 ToSlotIndex)
{
    if (!IsValidSlotIndex(FromSlotIndex) || !IsValidSlotIndex(ToSlotIndex) || FromSlotIndex == ToSlotIndex)
    {
        return false;
    }

    UClientItemInstance* SourceItem = Slots[FromSlotIndex].Item;
    UClientItemInstance* TargetItem = Slots[ToSlotIndex].Item;
    if (SourceItem == nullptr)
    {
        return false;
    }

    if (TargetItem != nullptr && TargetItem->CanStackWith(SourceItem))
    {
        TryStackIntoSlot(SourceItem, ToSlotIndex);
        if (SourceItem->GetRuntimeData().Count == 0)
        {
            ClearSlot(FromSlotIndex);
        }

        BroadcastInventoryUpdated();
        return true;
    }

    Slots[ToSlotIndex].Item = SourceItem;
    Slots[FromSlotIndex].Item = TargetItem;

    AssignItemToSlot(SourceItem, ToSlotIndex);
    if (TargetItem != nullptr)
    {
        AssignItemToSlot(TargetItem, FromSlotIndex);
    }
    else
    {
        ClearSlot(FromSlotIndex);
    }

    BroadcastInventoryUpdated();
    return true;
}

UClientItemInstance* UClientInventoryComponent::GetItemAt(int32 SlotIndex) const
{
    return IsValidSlotIndex(SlotIndex) ? Slots[SlotIndex].Item : nullptr;
}

int32 UClientInventoryComponent::FindItemSlotByKey(const FString& ItemKey) const
{
    for (int32 SlotIndex = 0; SlotIndex < Slots.Num(); ++SlotIndex)
    {
        const UClientItemInstance* Item = Slots[SlotIndex].Item;
        if (Item != nullptr && Item->GetRuntimeData().ItemKey == ItemKey)
        {
            return SlotIndex;
        }
    }

    return INDEX_NONE;
}

const TArray<FClientInventorySlotState>& UClientInventoryComponent::GetSlots() const
{
    return Slots;
}

bool UClientInventoryComponent::IsValidSlotIndex(int32 SlotIndex) const
{
    return Slots.IsValidIndex(SlotIndex);
}

bool UClientInventoryComponent::TryStackIntoSlot(UClientItemInstance* SourceItem, int32 SlotIndex)
{
    if (SourceItem == nullptr || !IsValidSlotIndex(SlotIndex))
    {
        return false;
    }

    UClientItemInstance* TargetItem = Slots[SlotIndex].Item;
    if (TargetItem == nullptr || !TargetItem->CanStackWith(SourceItem))
    {
        return false;
    }

    const int32 MaxStackCount = FMath::Max(1, TargetItem->GetStaticData().MaxStackCount);
    const int32 AvailableSpace = MaxStackCount - TargetItem->GetRuntimeData().Count;
    if (AvailableSpace <= 0)
    {
        return false;
    }

    const int32 MoveCount = FMath::Min(AvailableSpace, SourceItem->GetRuntimeData().Count);
    TargetItem->SetCount(TargetItem->GetRuntimeData().Count + MoveCount);
    SourceItem->SetCount(SourceItem->GetRuntimeData().Count - MoveCount);
    return true;
}

void UClientInventoryComponent::AssignItemToSlot(UClientItemInstance* Item, int32 SlotIndex)
{
    if (Item == nullptr || !IsValidSlotIndex(SlotIndex))
    {
        return;
    }

    Slots[SlotIndex].Item = Item;

    FClientItemLocation NewLocation = Item->GetRuntimeData().Location;
    NewLocation.StorageKind = EClientItemStorageKind::Inventory;
    NewLocation.SlotIndex = SlotIndex;
    NewLocation.SecondaryIndex = INDEX_NONE;
    NewLocation.EquipSlot = EClientEquipSlot::None;
    NewLocation.SkillHotSlot = EClientSkillHotSlot::None;
    Item->SetLocation(NewLocation);
}

void UClientInventoryComponent::ClearSlot(int32 SlotIndex)
{
    if (!IsValidSlotIndex(SlotIndex))
    {
        return;
    }

    if (UClientItemInstance* Item = Slots[SlotIndex].Item)
    {
        FClientItemLocation NewLocation = Item->GetRuntimeData().Location;
        NewLocation.StorageKind = EClientItemStorageKind::None;
        NewLocation.SlotIndex = INDEX_NONE;
        NewLocation.SecondaryIndex = INDEX_NONE;
        NewLocation.EquipSlot = EClientEquipSlot::None;
        NewLocation.SkillHotSlot = EClientSkillHotSlot::None;
        Item->SetLocation(NewLocation);
    }

    Slots[SlotIndex].Item = nullptr;
}

void UClientInventoryComponent::BroadcastInventoryUpdated()
{
    OnInventoryUpdated.Broadcast();
}
