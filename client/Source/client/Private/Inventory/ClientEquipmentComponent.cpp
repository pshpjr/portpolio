#include "Inventory/ClientEquipmentComponent.h"

#include "Inventory/ClientItemInstance.h"

UClientEquipmentComponent::UClientEquipmentComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    InitializeDefaultSlots();
}

void UClientEquipmentComponent::InitializeDefaultSlots()
{
    EquippedItems.Empty();

    static const TArray<EClientEquipSlot> DefaultSlots = {
        EClientEquipSlot::Weapon,
        EClientEquipSlot::Head,
        EClientEquipSlot::Chest,
        EClientEquipSlot::Hands,
        EClientEquipSlot::Legs,
        EClientEquipSlot::Feet,
        EClientEquipSlot::Necklace,
        EClientEquipSlot::RingLeft,
        EClientEquipSlot::RingRight
    };

    for (EClientEquipSlot Slot : DefaultSlots)
    {
        EquippedItems.Add(Slot, nullptr);
    }
}

bool UClientEquipmentComponent::CanEquipItem(const UClientItemInstance* Item, EClientEquipSlot Slot) const
{
    return Item != nullptr
        && Slot != EClientEquipSlot::None
        && Item->CanEquipInSlot(Slot)
        && (!EquippedItems.Contains(Slot) || EquippedItems.FindRef(Slot) == nullptr);
}

bool UClientEquipmentComponent::EquipItem(UClientItemInstance* Item, EClientEquipSlot Slot)
{
    if (!CanEquipItem(Item, Slot))
    {
        return false;
    }

    EquippedItems.FindOrAdd(Slot) = Item;
    UpdateItemLocation(Item, Slot);
    OnEquipmentUpdated.Broadcast();
    return true;
}

UClientItemInstance* UClientEquipmentComponent::UnequipItem(EClientEquipSlot Slot)
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
        NewLocation.EquipSlot = EClientEquipSlot::None;
        RemovedItem->SetLocation(NewLocation);
    }

    OnEquipmentUpdated.Broadcast();
    return RemovedItem;
}

UClientItemInstance* UClientEquipmentComponent::GetEquippedItem(EClientEquipSlot Slot) const
{
    return EquippedItems.FindRef(Slot);
}

EClientWeaponType UClientEquipmentComponent::GetEquippedWeaponType() const
{
    if (const UClientItemInstance* WeaponItem = EquippedItems.FindRef(EClientEquipSlot::Weapon))
    {
        return WeaponItem->GetStaticData().WeaponType;
    }

    return EClientWeaponType::None;
}

const TMap<EClientEquipSlot, TObjectPtr<UClientItemInstance>>& UClientEquipmentComponent::GetEquippedItems() const
{
    return EquippedItems;
}

void UClientEquipmentComponent::UpdateItemLocation(UClientItemInstance* Item, EClientEquipSlot Slot) const
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
