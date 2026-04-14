#include "ClientCharacterSubsystem.h"

#include "Inventory/ClientInventoryComponent.h"
#include "Inventory/ClientEquipmentComponent.h"
#include "Inventory/ClientSkillHotbarComponent.h"
#include "Inventory/ClientStorageComponent.h"
#include "Inventory/ClientConsumableQuickSlotComponent.h"
#include "Inventory/ClientCurrencyComponent.h"

void UClientCharacterSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    Inventory        = NewObject<UClientInventoryComponent>(this);
    Equipment        = NewObject<UClientEquipmentComponent>(this);
    SkillHotbar      = NewObject<UClientSkillHotbarComponent>(this);
    Storage          = NewObject<UClientStorageComponent>(this);
    ConsumableQuickSlot = NewObject<UClientConsumableQuickSlotComponent>(this);
    Currency            = NewObject<UClientCurrencyComponent>(this);
}
