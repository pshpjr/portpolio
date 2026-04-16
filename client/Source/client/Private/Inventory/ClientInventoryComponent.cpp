#include "Inventory/ClientInventoryComponent.h"

#include "Inventory/ClientItemInstance.h"

UClientInventoryComponent::UClientInventoryComponent()
{
}

bool UClientInventoryComponent::AddItem(UClientItemInstance* Item)
{
    if (Item == nullptr || !Item->CanMoveToStorage(EClientItemStorageKind::Inventory))
    {
        return false;
    }

    if (Item->IsStackable() && TryStackIntoExisting(Item))
    {
        // 기존 스택에 합쳐진 경우 — 합쳐진 대상 아이템을 찾아 통보한다.
        UClientItemInstance* MergedTarget = FindItemByKey(Item->GetRuntimeData().ItemKey);
        BroadcastItemUpdated(MergedTarget);
        return true;
    }

    // 새 슬롯이 필요한데 열린 용량이 꽉 찼으면 실패.
    if (Items.Num() >= CurrentCapacity)
    {
        return false;
    }

    FClientItemLocation NewLocation = Item->GetRuntimeData().Location;
    NewLocation.StorageKind = EClientItemStorageKind::Inventory;
    NewLocation.SlotIndex = INDEX_NONE;
    Item->SetLocation(NewLocation);

    Items.Add(Item);
    SortItems();
    BroadcastItemUpdated(Item);
    return true;
}

void UClientInventoryComponent::SetCurrentCapacity(int32 NewCapacity)
{
    const int32 Clamped = FMath::Clamp(NewCapacity, 0, GClientInventoryMaxCapacity);
    if (Clamped == CurrentCapacity)
    {
        return;
    }
    CurrentCapacity = Clamped;
    // 전체 재렌더 신호: UI가 잠금 상태를 다시 계산하도록 null 브로드캐스트.
    BroadcastItemUpdated(nullptr);
}

bool UClientInventoryComponent::RemoveItemByKey(const FString& ItemKey)
{
    const int32 Index = Items.IndexOfByPredicate([&ItemKey](const TObjectPtr<UClientItemInstance>& Item)
    {
        return Item != nullptr && Item->GetRuntimeData().ItemKey == ItemKey;
    });

    if (Index == INDEX_NONE)
    {
        return false;
    }

    UClientItemInstance* RemovedItem = Items[Index];
    Items.RemoveAt(Index);

    if (RemovedItem != nullptr)
    {
        FClientItemLocation ClearedLocation = RemovedItem->GetRuntimeData().Location;
        ClearedLocation.StorageKind = EClientItemStorageKind::None;
        ClearedLocation.SlotIndex = INDEX_NONE;
        RemovedItem->SetLocation(ClearedLocation);
    }

    // 제거는 목록 순서가 바뀌므로 null로 전체 재갱신 신호를 보낸다.
    BroadcastItemUpdated(nullptr);
    return true;
}

const TArray<UClientItemInstance*>& UClientInventoryComponent::GetItems() const
{
    // TArray<TObjectPtr<T>>는 TArray<T*>로 암시적 변환이 안 되므로 reinterpret_cast 사용.
    return reinterpret_cast<const TArray<UClientItemInstance*>&>(Items);
}

UClientItemInstance* UClientInventoryComponent::FindItemByKey(const FString& ItemKey) const
{
    for (const TObjectPtr<UClientItemInstance>& Item : Items)
    {
        if (Item != nullptr && Item->GetRuntimeData().ItemKey == ItemKey)
        {
            return Item;
        }
    }
    return nullptr;
}

bool UClientInventoryComponent::TryStackIntoExisting(UClientItemInstance* Item)
{
    for (const TObjectPtr<UClientItemInstance>& Existing : Items)
    {
        if (Existing == nullptr || !Existing->CanStackWith(Item))
        {
            continue;
        }

        const int32 MaxStack = FMath::Max(1, Existing->GetStaticData().MaxStackCount);
        const int32 Available = MaxStack - Existing->GetRuntimeData().Count;
        if (Available <= 0)
        {
            continue;
        }

        const int32 MoveCount = FMath::Min(Available, Item->GetRuntimeData().Count);
        Existing->SetCount(Existing->GetRuntimeData().Count + MoveCount);
        Item->SetCount(Item->GetRuntimeData().Count - MoveCount);
        return Item->GetRuntimeData().Count == 0;
    }
    return false;
}

void UClientInventoryComponent::SortItems()
{
    // 정렬 기준: 카테고리(오름차순) → 등급(내림차순) → 표시 이름(가나다순)
    Items.Sort([](const UClientItemInstance& A, const UClientItemInstance& B)
    {
        const FItemTableRow& SA = A.GetStaticData();
        const FItemTableRow& SB = B.GetStaticData();

        if (SA.ItemCategory != SB.ItemCategory)
        {
            return static_cast<uint8>(SA.ItemCategory) < static_cast<uint8>(SB.ItemCategory);
        }

        if (SA.Rarity != SB.Rarity)
        {
            return static_cast<uint8>(SA.Rarity) > static_cast<uint8>(SB.Rarity);
        }

        return SA.ItemName < SB.ItemName;
    });
}

void UClientInventoryComponent::BroadcastItemUpdated(UClientItemInstance* ChangedItem)
{
    OnInventoryUpdated.Broadcast(ChangedItem);
}
