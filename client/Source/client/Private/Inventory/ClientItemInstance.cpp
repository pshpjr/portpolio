#include "Inventory/ClientItemInstance.h"

void UClientItemInstance::InitializeFromData(const FItemTableRow& InStaticData, const FClientItemRuntimeData& InRuntimeData)
{
    StaticData = InStaticData;
    RuntimeData = InRuntimeData;

    SetCount(RuntimeData.Count);
    SetDurability(RuntimeData.Durability);
    SetRemainingTradeCount(RuntimeData.RemainingTradeCount);
}

const FItemTableRow& UClientItemInstance::GetStaticData() const
{
    return StaticData;
}

const FClientItemRuntimeData& UClientItemInstance::GetRuntimeData() const
{
    return RuntimeData;
}

bool UClientItemInstance::IsStackable() const
{
    return StaticData.MaxStackCount > 1;
}

bool UClientItemInstance::IsBroken() const
{
    return StaticData.MaxDurability > 0 && RuntimeData.Durability <= 0;
}

bool UClientItemInstance::CanStackWith(const UClientItemInstance* OtherItem) const
{
    if (OtherItem == nullptr)
    {
        return false;
    }

    const FItemTableRow& OtherStaticData = OtherItem->GetStaticData();
    const FClientItemRuntimeData& OtherRuntimeData = OtherItem->GetRuntimeData();

    return StaticData.ItemId == OtherStaticData.ItemId
        && StaticData.MaxStackCount > 1
        && RuntimeData.Durability == OtherRuntimeData.Durability
        && RuntimeData.bIsAccountBound == OtherRuntimeData.bIsAccountBound
        && RuntimeData.RemainingTradeCount == OtherRuntimeData.RemainingTradeCount;
}

bool UClientItemInstance::CanEquipInSlot(EEquipSlot TargetSlot) const
{
    return StaticData.EquipSlot != EEquipSlot::None && StaticData.EquipSlot == TargetSlot;
}

bool UClientItemInstance::CanMoveToStorage(EClientItemStorageKind TargetStorageKind) const
{
    if (TargetStorageKind == EClientItemStorageKind::SharedStorage)
    {
        return StaticData.bCanUseSharedStorage;
    }

    return TargetStorageKind != EClientItemStorageKind::None;
}

void UClientItemInstance::SetCount(int32 NewCount)
{
    const int32 MaxStackCount = FMath::Max(1, StaticData.MaxStackCount);
    RuntimeData.Count = FMath::Clamp(NewCount, 0, MaxStackCount);
}

void UClientItemInstance::SetDurability(int32 NewDurability)
{
    const int32 MaxDurability = FMath::Max(StaticData.MaxDurability, 0);
    RuntimeData.Durability = MaxDurability > 0
        ? FMath::Clamp(NewDurability, 0, MaxDurability)
        : 0;
}

void UClientItemInstance::SetRemainingTradeCount(int32 NewTradeCount)
{
    const int32 MaxTradeCount = FMath::Max(StaticData.MaxTradeCount, 0);
    RuntimeData.RemainingTradeCount = FMath::Clamp(NewTradeCount, 0, MaxTradeCount);
}

void UClientItemInstance::SetAccountBound(bool bNewAccountBound)
{
    RuntimeData.bIsAccountBound = bNewAccountBound;
}

void UClientItemInstance::SetLocation(const FClientItemLocation& NewLocation)
{
    RuntimeData.Location = NewLocation;
}
