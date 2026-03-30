#include "UI/ClientStorageWidgetBase.h"

#include "Inventory/ClientStorageComponent.h"

void UClientStorageWidgetBase::BindStorageComponent(UClientStorageComponent* InStorageComponent)
{
    if (StorageComponent == InStorageComponent)
    {
        RefreshFromStorage();
        return;
    }

    if (StorageComponent != nullptr)
    {
        StorageComponent->OnStorageUpdated.RemoveDynamic(this, &UClientStorageWidgetBase::HandleStorageUpdated);
    }

    StorageComponent = InStorageComponent;

    if (StorageComponent != nullptr)
    {
        StorageComponent->OnStorageUpdated.AddDynamic(this, &UClientStorageWidgetBase::HandleStorageUpdated);
    }

    RefreshFromStorage();
}

UClientStorageComponent* UClientStorageWidgetBase::GetStorageComponent() const
{
    return StorageComponent;
}

void UClientStorageWidgetBase::NativeDestruct()
{
    if (StorageComponent != nullptr)
    {
        StorageComponent->OnStorageUpdated.RemoveDynamic(this, &UClientStorageWidgetBase::HandleStorageUpdated);
    }

    Super::NativeDestruct();
}

void UClientStorageWidgetBase::HandleStorageUpdated()
{
    RefreshFromStorage();
}
