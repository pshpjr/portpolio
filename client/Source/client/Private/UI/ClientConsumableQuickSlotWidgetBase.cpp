#include "UI/ClientConsumableQuickSlotWidgetBase.h"

#include "Inventory/ClientConsumableQuickSlotComponent.h"

void UClientConsumableQuickSlotWidgetBase::BindConsumableQuickSlotComponent(UClientConsumableQuickSlotComponent* InComponent)
{
    if (ConsumableQuickSlotComponent != nullptr)
    {
        ConsumableQuickSlotComponent->OnConsumableQuickSlotUpdated.RemoveDynamic(this, &UClientConsumableQuickSlotWidgetBase::HandleConsumableQuickSlotUpdated);
    }

    ConsumableQuickSlotComponent = InComponent;

    if (ConsumableQuickSlotComponent != nullptr)
    {
        ConsumableQuickSlotComponent->OnConsumableQuickSlotUpdated.AddDynamic(this, &UClientConsumableQuickSlotWidgetBase::HandleConsumableQuickSlotUpdated);
        RefreshFromConsumableQuickSlot();
    }
}

UClientConsumableQuickSlotComponent* UClientConsumableQuickSlotWidgetBase::GetConsumableQuickSlotComponent() const
{
    return ConsumableQuickSlotComponent;
}

void UClientConsumableQuickSlotWidgetBase::NativeDestruct()
{
    if (ConsumableQuickSlotComponent != nullptr)
    {
        ConsumableQuickSlotComponent->OnConsumableQuickSlotUpdated.RemoveDynamic(this, &UClientConsumableQuickSlotWidgetBase::HandleConsumableQuickSlotUpdated);
        ConsumableQuickSlotComponent = nullptr;
    }

    Super::NativeDestruct();
}

void UClientConsumableQuickSlotWidgetBase::HandleConsumableQuickSlotUpdated()
{
    RefreshFromConsumableQuickSlot();
}
