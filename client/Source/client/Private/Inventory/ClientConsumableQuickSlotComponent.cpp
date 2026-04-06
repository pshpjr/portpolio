#include "Inventory/ClientConsumableQuickSlotComponent.h"

#include "Inventory/ClientItemInstance.h"

UClientConsumableQuickSlotComponent::UClientConsumableQuickSlotComponent()
{
    SlotItems.Add(EClientConsumableSlot::Slot1, nullptr);
    SlotItems.Add(EClientConsumableSlot::Slot2, nullptr);
}

bool UClientConsumableQuickSlotComponent::AssignConsumable(UClientItemInstance* Item, EClientConsumableSlot Slot)
{
    if (!IsValidSlot(Slot) || Item == nullptr)
    {
        return false;
    }

    if (Item->GetStaticData().ItemCategory != EItemCategory::Consumable)
    {
        return false;
    }

    if (Item->GetStaticData().MaxStackCount > GClientConsumableSlotMaxStack)
    {
        return false;
    }

    SlotItems[Slot] = Item;

    FClientItemLocation NewLocation = Item->GetRuntimeData().Location;
    NewLocation.StorageKind = EClientItemStorageKind::ConsumableQuickSlot;
    NewLocation.SlotIndex = static_cast<int32>(Slot);
    Item->SetLocation(NewLocation);

    BroadcastUpdated();
    return true;
}

UClientItemInstance* UClientConsumableQuickSlotComponent::ClearSlot(EClientConsumableSlot Slot)
{
    if (!IsValidSlot(Slot))
    {
        return nullptr;
    }

    UClientItemInstance* RemovedItem = SlotItems.FindRef(Slot);
    SlotItems[Slot] = nullptr;

    if (RemovedItem != nullptr)
    {
        FClientItemLocation ClearedLocation = RemovedItem->GetRuntimeData().Location;
        ClearedLocation.StorageKind = EClientItemStorageKind::None;
        ClearedLocation.SlotIndex = INDEX_NONE;
        RemovedItem->SetLocation(ClearedLocation);
    }

    BroadcastUpdated();
    return RemovedItem;
}

bool UClientConsumableQuickSlotComponent::ConsumeFromSlot(EClientConsumableSlot Slot)
{
    if (!IsValidSlot(Slot))
    {
        return false;
    }

    UClientItemInstance* Item = SlotItems.FindRef(Slot);
    if (Item == nullptr || Item->GetRuntimeData().Count <= 0)
    {
        return false;
    }

    Item->SetCount(Item->GetRuntimeData().Count - 1);

    if (Item->GetRuntimeData().Count == 0)
    {
        SlotItems[Slot] = nullptr;
    }

    BroadcastUpdated();
    return true;
}

UClientItemInstance* UClientConsumableQuickSlotComponent::GetItemAtSlot(EClientConsumableSlot Slot) const
{
    return IsValidSlot(Slot) ? SlotItems.FindRef(Slot) : nullptr;
}

int32 UClientConsumableQuickSlotComponent::GetCountAtSlot(EClientConsumableSlot Slot) const
{
    const UClientItemInstance* Item = GetItemAtSlot(Slot);
    return Item ? Item->GetRuntimeData().Count : 0;
}

bool UClientConsumableQuickSlotComponent::IsValidSlot(EClientConsumableSlot Slot) const
{
    return Slot != EClientConsumableSlot::None && SlotItems.Contains(Slot);
}

void UClientConsumableQuickSlotComponent::BroadcastUpdated()
{
    OnConsumableQuickSlotUpdated.Broadcast();
}
