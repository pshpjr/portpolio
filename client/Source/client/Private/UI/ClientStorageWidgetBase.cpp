#include "UI/ClientStorageWidgetBase.h"

#include "ClientCharacterSubsystem.h"
#include "Blueprint/WidgetTree.h"
#include "Components/UniformGridPanel.h"
#include "Components/SizeBox.h"
#include "Inventory/ClientInventoryTypes.h"
#include "Inventory/ClientItemInstance.h"
#include "Inventory/ClientStorageComponent.h"
#include "UI/InventorySlot.h"

void UClientStorageWidgetBase::NativePreConstruct()
{
    Super::NativePreConstruct();
    RebuildSlots();
}

void UClientStorageWidgetBase::NativeConstruct()
{
    Super::NativeConstruct();

    RebuildSlots();

    if (UGameInstance* GI = GetGameInstance())
    {
        if (UClientCharacterSubsystem* CharSub = GI->GetSubsystem<UClientCharacterSubsystem>())
        {
            BindStorageComponent(CharSub->GetStorage());
        }
    }
}

void UClientStorageWidgetBase::BindStorageComponent(UClientStorageComponent* InStorageComponent)
{
    if (StorageComponent == InStorageComponent)
    {
        RefreshSlots();
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

    RefreshSlots();
    RefreshFromStorage();
}

UClientStorageComponent* UClientStorageWidgetBase::GetStorageComponent() const
{
    return StorageComponent;
}

void UClientStorageWidgetBase::SetActiveTabIndex(int32 NewTabIndex)
{
    if (NewTabIndex == ActiveTabIndex)
    {
        return;
    }
    ActiveTabIndex = FMath::Max(0, NewTabIndex);
    RefreshSlots();
    RefreshFromStorage();
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
    RefreshSlots();
    RefreshFromStorage();
}

void UClientStorageWidgetBase::RebuildSlots()
{
    if (SlotContainer == nullptr || !SlotClass)
    {
        return;
    }

    SlotContainer->ClearChildren();
    SlotContainer->SetSlotPadding(SlotPadding);
    SlotWidgets.Reset(GClientStorageTabMaxCapacity);

    const int32 Columns = FMath::Max(1, NumColumns);
    for (int32 Index = 0; Index < GClientStorageTabMaxCapacity; ++Index)
    {
        UInventorySlot* NewSlot = CreateWidget<UInventorySlot>(this, SlotClass);
        if (NewSlot == nullptr)
        {
            continue;
        }

        NewSlot->ApplyPixelSize(SlotSize);

        const int32 Row = Index / Columns;
        const int32 Column = Index % Columns;

        if (SlotSize.X > 0.f || SlotSize.Y > 0.f)
        {
            USizeBox* Wrapper = WidgetTree ? WidgetTree->ConstructWidget<USizeBox>() : nullptr;
            if (Wrapper != nullptr)
            {
                if (SlotSize.X > 0.f) { Wrapper->SetWidthOverride(SlotSize.X); }
                if (SlotSize.Y > 0.f) { Wrapper->SetHeightOverride(SlotSize.Y); }
                Wrapper->AddChild(NewSlot);
                SlotContainer->AddChildToUniformGrid(Wrapper, Row, Column);
            }
            else
            {
                SlotContainer->AddChildToUniformGrid(NewSlot, Row, Column);
            }
        }
        else
        {
            SlotContainer->AddChildToUniformGrid(NewSlot, Row, Column);
        }

        SlotWidgets.Add(NewSlot);
    }

    RefreshSlots();
}

void UClientStorageWidgetBase::RefreshSlots()
{
    if (SlotWidgets.Num() == 0)
    {
        return;
    }

    const int32 CurrentCapacity = StorageComponent ? StorageComponent->GetTabCurrentCapacity(ActiveTabIndex) : 0;

    for (int32 Index = 0; Index < SlotWidgets.Num(); ++Index)
    {
        UInventorySlot* SlotWidget = SlotWidgets[Index];
        if (SlotWidget == nullptr)
        {
            continue;
        }

        if (Index >= CurrentCapacity)
        {
            SlotWidget->SetLocked(true);
            continue;
        }

        SlotWidget->SetLocked(false);
        UClientItemInstance* Item = StorageComponent ? StorageComponent->GetItemAt(ActiveTabIndex, Index) : nullptr;
        SlotWidget->SetItem(Item);
    }
}
