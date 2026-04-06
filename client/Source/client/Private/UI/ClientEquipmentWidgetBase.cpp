#include "UI/ClientEquipmentWidgetBase.h"

#include "ClientCharacterSubsystem.h"
#include "Inventory/ClientEquipmentComponent.h"

void UClientEquipmentWidgetBase::NativeConstruct()
{
    Super::NativeConstruct();

    if (UGameInstance* GI = GetOwningGameInstance())
    {
        if (UClientCharacterSubsystem* CharSub = GI->GetSubsystem<UClientCharacterSubsystem>())
        {
            BindEquipmentComponent(CharSub->GetEquipment());
        }
    }
}

void UClientEquipmentWidgetBase::BindEquipmentComponent(UClientEquipmentComponent* InEquipmentComponent)
{
    if (EquipmentComponent == InEquipmentComponent)
    {
        RefreshFromEquipment();
        return;
    }

    if (EquipmentComponent != nullptr)
    {
        EquipmentComponent->OnEquipmentUpdated.RemoveDynamic(this, &UClientEquipmentWidgetBase::HandleEquipmentUpdated);
    }

    EquipmentComponent = InEquipmentComponent;

    if (EquipmentComponent != nullptr)
    {
        EquipmentComponent->OnEquipmentUpdated.AddDynamic(this, &UClientEquipmentWidgetBase::HandleEquipmentUpdated);
    }

    RefreshFromEquipment();
}

UClientEquipmentComponent* UClientEquipmentWidgetBase::GetEquipmentComponent() const
{
    return EquipmentComponent;
}

void UClientEquipmentWidgetBase::NativeDestruct()
{
    if (EquipmentComponent != nullptr)
    {
        EquipmentComponent->OnEquipmentUpdated.RemoveDynamic(this, &UClientEquipmentWidgetBase::HandleEquipmentUpdated);
    }

    Super::NativeDestruct();
}

void UClientEquipmentWidgetBase::HandleEquipmentUpdated()
{
    RefreshFromEquipment();
}
