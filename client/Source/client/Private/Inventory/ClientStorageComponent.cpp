#include "Inventory/ClientStorageComponent.h"

#include "Inventory/ClientItemInstance.h"

UClientStorageComponent::UClientStorageComponent()
{
}

void UClientStorageComponent::InitializeStorage(const TArray<FClientStorageTabDescriptor>& TabDescriptors)
{
    Tabs.Empty();

    for (const FClientStorageTabDescriptor& TabDescriptor : TabDescriptors)
    {
        FClientStorageTabState TabState;
        TabState.TabId = TabDescriptor.TabId;
        TabState.Capacity = FMath::Max(0, TabDescriptor.Capacity);
        TabState.Slots.SetNum(TabState.Capacity);
        Tabs.Add(MoveTemp(TabState));
    }
}

void UClientStorageComponent::InitializeDefaultSharedStorage(int32 TabCount, int32 SlotsPerTab)
{
    TArray<FClientStorageTabDescriptor> TabDescriptors;
    for (int32 TabIndex = 0; TabIndex < FMath::Max(1, TabCount); ++TabIndex)
    {
        FClientStorageTabDescriptor Descriptor;
        Descriptor.TabId = *FString::Printf(TEXT("Shared_%d"), TabIndex);
        Descriptor.Capacity = FMath::Max(1, SlotsPerTab);
        TabDescriptors.Add(Descriptor);
    }

    InitializeStorage(TabDescriptors);
}

bool UClientStorageComponent::AddItemToTab(UClientItemInstance* Item, int32 TabIndex, int32 PreferredSlotIndex)
{
    if (Item == nullptr || !IsValidTabIndex(TabIndex) || !Item->CanMoveToStorage(EClientItemStorageKind::SharedStorage))
    {
        return false;
    }

    if (Item->IsStackable())
    {
        for (int32 SlotIndex = 0; SlotIndex < Tabs[TabIndex].Slots.Num(); ++SlotIndex)
        {
            if (TryStackIntoSlot(Item, TabIndex, SlotIndex) && Item->GetRuntimeData().Count == 0)
            {
                BroadcastStorageUpdated();
                return true;
            }
        }
    }

    if (IsValidSlotIndex(TabIndex, PreferredSlotIndex) && Tabs[TabIndex].Slots[PreferredSlotIndex] == nullptr)
    {
        AssignItemToSlot(Item, TabIndex, PreferredSlotIndex);
        BroadcastStorageUpdated();
        return true;
    }

    for (int32 SlotIndex = 0; SlotIndex < Tabs[TabIndex].Slots.Num(); ++SlotIndex)
    {
        if (Tabs[TabIndex].Slots[SlotIndex] == nullptr)
        {
            AssignItemToSlot(Item, TabIndex, SlotIndex);
            BroadcastStorageUpdated();
            return true;
        }
    }

    return false;
}

bool UClientStorageComponent::MoveItem(int32 FromTabIndex, int32 FromSlotIndex, int32 ToTabIndex, int32 ToSlotIndex)
{
    if (!IsValidSlotIndex(FromTabIndex, FromSlotIndex) || !IsValidSlotIndex(ToTabIndex, ToSlotIndex))
    {
        return false;
    }

    UClientItemInstance* SourceItem = Tabs[FromTabIndex].Slots[FromSlotIndex];
    UClientItemInstance* TargetItem = Tabs[ToTabIndex].Slots[ToSlotIndex];
    if (SourceItem == nullptr)
    {
        return false;
    }

    if (TargetItem != nullptr && TargetItem->CanStackWith(SourceItem))
    {
        TryStackIntoSlot(SourceItem, ToTabIndex, ToSlotIndex);
        if (SourceItem->GetRuntimeData().Count == 0)
        {
            ClearSlot(FromTabIndex, FromSlotIndex);
        }

        BroadcastStorageUpdated();
        return true;
    }

    Tabs[ToTabIndex].Slots[ToSlotIndex] = SourceItem;
    Tabs[FromTabIndex].Slots[FromSlotIndex] = TargetItem;

    AssignItemToSlot(SourceItem, ToTabIndex, ToSlotIndex);
    if (TargetItem != nullptr)
    {
        AssignItemToSlot(TargetItem, FromTabIndex, FromSlotIndex);
    }
    else
    {
        ClearSlot(FromTabIndex, FromSlotIndex);
    }

    BroadcastStorageUpdated();
    return true;
}

UClientItemInstance* UClientStorageComponent::RemoveItem(int32 TabIndex, int32 SlotIndex)
{
    if (!IsValidSlotIndex(TabIndex, SlotIndex))
    {
        return nullptr;
    }

    UClientItemInstance* RemovedItem = Tabs[TabIndex].Slots[SlotIndex];
    ClearSlot(TabIndex, SlotIndex);
    BroadcastStorageUpdated();
    return RemovedItem;
}

UClientItemInstance* UClientStorageComponent::GetItemAt(int32 TabIndex, int32 SlotIndex) const
{
    return IsValidSlotIndex(TabIndex, SlotIndex) ? Tabs[TabIndex].Slots[SlotIndex] : nullptr;
}

const TArray<FClientStorageTabState>& UClientStorageComponent::GetTabs() const
{
    return Tabs;
}

bool UClientStorageComponent::IsValidTabIndex(int32 TabIndex) const
{
    return Tabs.IsValidIndex(TabIndex);
}

bool UClientStorageComponent::IsValidSlotIndex(int32 TabIndex, int32 SlotIndex) const
{
    return IsValidTabIndex(TabIndex) && Tabs[TabIndex].Slots.IsValidIndex(SlotIndex);
}

bool UClientStorageComponent::TryStackIntoSlot(UClientItemInstance* SourceItem, int32 TabIndex, int32 SlotIndex)
{
    if (SourceItem == nullptr || !IsValidSlotIndex(TabIndex, SlotIndex))
    {
        return false;
    }

    UClientItemInstance* TargetItem = Tabs[TabIndex].Slots[SlotIndex];
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

void UClientStorageComponent::AssignItemToSlot(UClientItemInstance* Item, int32 TabIndex, int32 SlotIndex)
{
    if (Item == nullptr || !IsValidSlotIndex(TabIndex, SlotIndex))
    {
        return;
    }

    Tabs[TabIndex].Slots[SlotIndex] = Item;

    FClientItemLocation NewLocation = Item->GetRuntimeData().Location;
    NewLocation.StorageKind = EClientItemStorageKind::SharedStorage;
    NewLocation.SlotIndex = SlotIndex;
    NewLocation.SecondaryIndex = TabIndex;
    NewLocation.EquipSlot = EEquipSlot::None;
    NewLocation.SkillHotSlot = EClientSkillHotSlot::None;
    Item->SetLocation(NewLocation);
}

void UClientStorageComponent::ClearSlot(int32 TabIndex, int32 SlotIndex)
{
    if (!IsValidSlotIndex(TabIndex, SlotIndex))
    {
        return;
    }

    if (UClientItemInstance* Item = Tabs[TabIndex].Slots[SlotIndex])
    {
        FClientItemLocation NewLocation = Item->GetRuntimeData().Location;
        NewLocation.StorageKind = EClientItemStorageKind::None;
        NewLocation.SlotIndex = INDEX_NONE;
        NewLocation.SecondaryIndex = INDEX_NONE;
        NewLocation.EquipSlot = EEquipSlot::None;
        NewLocation.SkillHotSlot = EClientSkillHotSlot::None;
        Item->SetLocation(NewLocation);
    }

    Tabs[TabIndex].Slots[SlotIndex] = nullptr;
}

void UClientStorageComponent::BroadcastStorageUpdated()
{
    OnStorageUpdated.Broadcast();
}
