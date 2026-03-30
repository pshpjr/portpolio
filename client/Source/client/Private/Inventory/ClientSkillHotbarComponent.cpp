#include "Inventory/ClientSkillHotbarComponent.h"

UClientSkillHotbarComponent::UClientSkillHotbarComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UClientSkillHotbarComponent::SetEquippedWeaponType(EClientWeaponType InWeaponType)
{
    EquippedWeaponType = InWeaponType;
    BroadcastHotbarUpdated();
}

bool UClientSkillHotbarComponent::CanRegisterSkillToSlot(const FClientSkillDefinition& SkillDefinition, EClientSkillHotSlot Slot) const
{
    if (Slot == EClientSkillHotSlot::None || SkillDefinition.SkillTid <= 0)
    {
        return false;
    }

    if (SkillDefinition.SkillType == EClientSkillType::Identity)
    {
        return Slot == EClientSkillHotSlot::Identity;
    }

    if (!IsActiveSkillSlot(Slot))
    {
        return false;
    }

    return SkillDefinition.RequiredWeaponType == EClientWeaponType::None
        || EquippedWeaponType == EClientWeaponType::None
        || SkillDefinition.RequiredWeaponType == EquippedWeaponType;
}

bool UClientSkillHotbarComponent::RegisterSkillToSlot(const FClientSkillDefinition& SkillDefinition, EClientSkillHotSlot Slot)
{
    if (!CanRegisterSkillToSlot(SkillDefinition, Slot))
    {
        return false;
    }

    const EClientSkillHotSlot ExistingSlot = FindSlotBySkillKey(SkillDefinition.SkillKey);
    if (ExistingSlot != EClientSkillHotSlot::None)
    {
        SlotBindings.Remove(ExistingSlot);
    }

    SlotBindings.Add(Slot, SkillDefinition);
    BroadcastHotbarUpdated();
    return true;
}

bool UClientSkillHotbarComponent::ClearSlot(EClientSkillHotSlot Slot)
{
    if (!SlotBindings.Contains(Slot))
    {
        return false;
    }

    SlotBindings.Remove(Slot);
    BroadcastHotbarUpdated();
    return true;
}

bool UClientSkillHotbarComponent::TryGetSkillAtSlot(EClientSkillHotSlot Slot, FClientSkillDefinition& OutSkillDefinition) const
{
    if (const FClientSkillDefinition* SkillDefinition = SlotBindings.Find(Slot))
    {
        OutSkillDefinition = *SkillDefinition;
        return true;
    }

    return false;
}

EClientSkillHotSlot UClientSkillHotbarComponent::FindSlotBySkillKey(const FString& SkillKey) const
{
    for (const TPair<EClientSkillHotSlot, FClientSkillDefinition>& Pair : SlotBindings)
    {
        if (Pair.Value.SkillKey == SkillKey)
        {
            return Pair.Key;
        }
    }

    return EClientSkillHotSlot::None;
}

const TMap<EClientSkillHotSlot, FClientSkillDefinition>& UClientSkillHotbarComponent::GetSlotBindings() const
{
    return SlotBindings;
}

bool UClientSkillHotbarComponent::IsActiveSkillSlot(EClientSkillHotSlot Slot) const
{
    return Slot == EClientSkillHotSlot::Active1
        || Slot == EClientSkillHotSlot::Active2
        || Slot == EClientSkillHotSlot::Active3
        || Slot == EClientSkillHotSlot::Active4;
}

void UClientSkillHotbarComponent::BroadcastHotbarUpdated()
{
    OnSkillHotbarUpdated.Broadcast();
}
