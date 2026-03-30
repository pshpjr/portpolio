#include "UI/ClientInventoryWidgetBase.h"

#include "Inventory/ClientInventoryComponent.h"

void UClientInventoryWidgetBase::BindInventoryComponent(UClientInventoryComponent* InInventoryComponent)
{
    if (InventoryComponent == InInventoryComponent)
    {
        RefreshFromInventory();
        return;
    }

    if (InventoryComponent != nullptr)
    {
        InventoryComponent->OnInventoryUpdated.RemoveDynamic(this, &UClientInventoryWidgetBase::HandleInventoryUpdated);
    }

    InventoryComponent = InInventoryComponent;

    if (InventoryComponent != nullptr)
    {
        InventoryComponent->OnInventoryUpdated.AddDynamic(this, &UClientInventoryWidgetBase::HandleInventoryUpdated);
    }

    RefreshFromInventory();
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
    }

    Super::NativeDestruct();
}

void UClientInventoryWidgetBase::HandleInventoryUpdated()
{
    RefreshFromInventory();
}
