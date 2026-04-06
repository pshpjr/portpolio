#include "UI/ClientInventoryWidgetBase.h"

#include "Inventory/ClientInventoryComponent.h"
#include "Inventory/ClientItemInstance.h"

void UClientInventoryWidgetBase::BindInventoryComponent(UClientInventoryComponent* InInventoryComponent)
{
    if (InventoryComponent != nullptr)
    {
        InventoryComponent->OnInventoryUpdated.RemoveDynamic(this, &UClientInventoryWidgetBase::HandleInventoryUpdated);
    }

    InventoryComponent = InInventoryComponent;

    if (InventoryComponent == nullptr)
    {
        return;
    }

    InventoryComponent->OnInventoryUpdated.AddDynamic(this, &UClientInventoryWidgetBase::HandleInventoryUpdated);

    // 바인딩 직후 null을 전달해 전체 목록 초기화를 유도한다.
    OnInventoryUpdated(nullptr);
}

UClientInventoryComponent* UClientInventoryWidgetBase::GetInventoryComponent() const
{
    return InventoryComponent;
}

void UClientInventoryWidgetBase::NativeDestruct()
{
    if (InventoryComponent != nullptr)
    {
        InventoryComponent->OnInventoryUpdated.RemoveDynamic(this, &UClientInventoryWidgetBase::HandleInventoryUpdated);
        InventoryComponent = nullptr;
    }

    Super::NativeDestruct();
}

void UClientInventoryWidgetBase::HandleInventoryUpdated(UClientItemInstance* ChangedItem)
{
    OnInventoryUpdated(ChangedItem);
}
