#include "UI/ClientInventoryWidgetBase.h"

#include "ClientCharacterSubsystem.h"
#include "Blueprint/WidgetTree.h"
#include "Components/UniformGridPanel.h"
#include "Components/SizeBox.h"
#include "Inventory/ClientInventoryComponent.h"
#include "Inventory/ClientInventoryTypes.h"
#include "Inventory/ClientItemInstance.h"
#include "UI/InventorySlot.h"

void UClientInventoryWidgetBase::NativePreConstruct()
{
    Super::NativePreConstruct();
    // Designer 프리뷰에서도 슬롯이 보이도록 PreConstruct에서 한 번 빌드.
    RebuildSlots();
}

void UClientInventoryWidgetBase::NativeConstruct()
{
    Super::NativeConstruct();

    RebuildSlots();

    if (UGameInstance* GI = GetGameInstance())
    {
        if (UClientCharacterSubsystem* CharSub = GI->GetSubsystem<UClientCharacterSubsystem>())
        {
            BindInventoryComponent(CharSub->GetInventory());
        }
    }
}

void UClientInventoryWidgetBase::BindInventoryComponent(UClientInventoryComponent* InInventoryComponent)
{
    if (InventoryComponent != nullptr)
    {
        InventoryComponent->OnInventoryUpdated.RemoveDynamic(this, &UClientInventoryWidgetBase::HandleInventoryUpdated);
    }

    InventoryComponent = InInventoryComponent;

    if (InventoryComponent == nullptr)
    {
        RefreshSlots();
        return;
    }

    InventoryComponent->OnInventoryUpdated.AddDynamic(this, &UClientInventoryWidgetBase::HandleInventoryUpdated);

    RefreshSlots();
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
    RefreshSlots();
    OnInventoryUpdated(ChangedItem);
}

void UClientInventoryWidgetBase::RebuildSlots()
{
    if (SlotContainer == nullptr || !SlotClass)
    {
        return;
    }

    SlotContainer->ClearChildren();
    SlotContainer->SetSlotPadding(SlotPadding);
    SlotWidgets.Reset(GClientInventoryMaxCapacity);

    const int32 Columns = FMath::Max(1, NumColumns);
    for (int32 Index = 0; Index < GClientInventoryMaxCapacity; ++Index)
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

void UClientInventoryWidgetBase::SetActiveCategory(EItemCategory NewCategory)
{
    if (NewCategory == ActiveCategory)
    {
        return;
    }
    ActiveCategory = NewCategory;
    RefreshSlots();
}

void UClientInventoryWidgetBase::RefreshSlots()
{
    if (SlotWidgets.Num() == 0)
    {
        return;
    }

    const int32 CurrentCapacity = InventoryComponent ? InventoryComponent->GetCurrentCapacity() : 0;
    const TArray<UClientItemInstance*> EmptyItems;
    const TArray<UClientItemInstance*>& Items = InventoryComponent ? InventoryComponent->GetItems() : EmptyItems;

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

        UClientItemInstance* Item = Items.IsValidIndex(Index) ? Items[Index] : nullptr;
        // 카테고리 필터: None이면 통과, 아니면 카테고리 일치하는 아이템만 보여준다.
        if (ActiveCategory != EItemCategory::None && Item != nullptr &&
            Item->GetStaticData().ItemCategory != ActiveCategory)
        {
            Item = nullptr;
        }
        SlotWidget->SetItem(Item);
    }
}
