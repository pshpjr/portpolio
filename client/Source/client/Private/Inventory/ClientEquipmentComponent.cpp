#include "Inventory/ClientEquipmentComponent.h"

#include "Inventory/ClientItemInstance.h"

UClientEquipmentComponent::UClientEquipmentComponent()
{
    InitializeDefaultSlots();
}

void UClientEquipmentComponent::InitializeDefaultSlots()
{
    EquippedItems.Empty();

    static const TArray<EEquipSlot> DefaultSlots = {
        EEquipSlot::Weapon,
        EEquipSlot::Chest,
        EEquipSlot::Legs,
        EEquipSlot::Hands,
        EEquipSlot::Necklace,
        EEquipSlot::RingLeft,
        EEquipSlot::RingRight
    };

    for (EEquipSlot Slot : DefaultSlots)
    {
        EquippedItems.Add(Slot, nullptr);
    }
}

bool UClientEquipmentComponent::CanEquipItem(const UClientItemInstance* Item, EEquipSlot Slot) const
{
    return Item != nullptr
        && Slot != EEquipSlot::None
        && Item->CanEquipInSlot(Slot);
}

bool UClientEquipmentComponent::EquipItem(UClientItemInstance* Item, EEquipSlot Slot)
{
    UClientItemInstance* Previous = nullptr;
    return SwapEquipItem(Item, Slot, Previous);
}

bool UClientEquipmentComponent::SwapEquipItem(UClientItemInstance* Item, EEquipSlot Slot, UClientItemInstance*& OutPreviousItem)
{
    OutPreviousItem = nullptr;

    if (!CanEquipItem(Item, Slot))
    {
        return false;
    }

    OutPreviousItem = EquippedItems.FindRef(Slot);

    if (OutPreviousItem != nullptr)
    {
        FClientItemLocation ClearedLocation = OutPreviousItem->GetRuntimeData().Location;
        ClearedLocation.StorageKind = EClientItemStorageKind::None;
        ClearedLocation.SlotIndex = INDEX_NONE;
        ClearedLocation.SecondaryIndex = INDEX_NONE;
        ClearedLocation.EquipSlot = EEquipSlot::None;
        OutPreviousItem->SetLocation(ClearedLocation);
    }

    EquippedItems.FindOrAdd(Slot) = Item;
    UpdateItemLocation(Item, Slot);
    OnEquipmentUpdated.Broadcast();
    return true;
}

UClientItemInstance* UClientEquipmentComponent::UnequipItem(EEquipSlot Slot)
{
    if (!EquippedItems.Contains(Slot))
    {
        return nullptr;
    }

    UClientItemInstance* RemovedItem = EquippedItems.FindRef(Slot);
    EquippedItems[Slot] = nullptr;

    if (RemovedItem != nullptr)
    {
        FClientItemLocation NewLocation = RemovedItem->GetRuntimeData().Location;
        NewLocation.StorageKind = EClientItemStorageKind::None;
        NewLocation.SlotIndex = INDEX_NONE;
        NewLocation.SecondaryIndex = INDEX_NONE;
        NewLocation.EquipSlot = EEquipSlot::None;
        RemovedItem->SetLocation(NewLocation);
    }

    OnEquipmentUpdated.Broadcast();
    return RemovedItem;
}

UClientItemInstance* UClientEquipmentComponent::GetEquippedItem(EEquipSlot Slot) const
{
    return EquippedItems.FindRef(Slot);
}

EWeaponType UClientEquipmentComponent::GetEquippedWeaponType() const
{
    if (const UClientItemInstance* WeaponItem = EquippedItems.FindRef(EEquipSlot::Weapon))
    {
        return WeaponItem->GetStaticData().WeaponType;
    }

    return EWeaponType::None;
}

const TMap<EEquipSlot, TObjectPtr<UClientItemInstance>>& UClientEquipmentComponent::GetEquippedItems() const
{
    return EquippedItems;
}

void UClientEquipmentComponent::UpdateItemLocation(UClientItemInstance* Item, EEquipSlot Slot) const
{
    if (Item == nullptr)
    {
        return;
    }

    FClientItemLocation NewLocation = Item->GetRuntimeData().Location;
    NewLocation.StorageKind = EClientItemStorageKind::Equipment;
    NewLocation.SlotIndex = INDEX_NONE;
    NewLocation.SecondaryIndex = INDEX_NONE;
    NewLocation.EquipSlot = Slot;
    NewLocation.SkillHotSlot = EClientSkillHotSlot::None;
    Item->SetLocation(NewLocation);
}
