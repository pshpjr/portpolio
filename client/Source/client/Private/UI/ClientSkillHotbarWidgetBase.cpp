#include "UI/ClientSkillHotbarWidgetBase.h"

#include "ClientCharacterSubsystem.h"
#include "Inventory/ClientSkillHotbarComponent.h"

void UClientSkillHotbarWidgetBase::NativeConstruct()
{
    Super::NativeConstruct();

    if (UGameInstance* GI = GetOwningGameInstance())
    {
        if (UClientCharacterSubsystem* CharSub = GI->GetSubsystem<UClientCharacterSubsystem>())
        {
            BindSkillHotbarComponent(CharSub->GetSkillHotbar());
        }
    }
}

void UClientSkillHotbarWidgetBase::BindSkillHotbarComponent(UClientSkillHotbarComponent* InSkillHotbarComponent)
{
    if (SkillHotbarComponent == InSkillHotbarComponent)
    {
        RefreshFromSkillHotbar();
        return;
    }

    if (SkillHotbarComponent != nullptr)
    {
        SkillHotbarComponent->OnSkillHotbarUpdated.RemoveDynamic(this, &UClientSkillHotbarWidgetBase::HandleSkillHotbarUpdated);
    }

    SkillHotbarComponent = InSkillHotbarComponent;

    if (SkillHotbarComponent != nullptr)
    {
        SkillHotbarComponent->OnSkillHotbarUpdated.AddDynamic(this, &UClientSkillHotbarWidgetBase::HandleSkillHotbarUpdated);
    }

    RefreshFromSkillHotbar();
}

UClientSkillHotbarComponent* UClientSkillHotbarWidgetBase::GetSkillHotbarComponent() const
{
    return SkillHotbarComponent;
}

void UClientSkillHotbarWidgetBase::NativeDestruct()
{
    if (SkillHotbarComponent != nullptr)
    {
        SkillHotbarComponent->OnSkillHotbarUpdated.RemoveDynamic(this, &UClientSkillHotbarWidgetBase::HandleSkillHotbarUpdated);
    }

    Super::NativeDestruct();
}

void UClientSkillHotbarWidgetBase::HandleSkillHotbarUpdated()
{
    RefreshFromSkillHotbar();
}
