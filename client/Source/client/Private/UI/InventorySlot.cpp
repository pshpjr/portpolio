// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/InventorySlot.h"

void UInventorySlot::SetItem(UClientItemInstance* InItem)
{
    Item = InItem;

    if (Item == nullptr)
    {
        ItemIcon->SetVisibility(ESlateVisibility::Hidden);
        ItemCountText->SetVisibility(ESlateVisibility::Hidden);
        return;
    }

    const FString& IconKey = Item->GetStaticData().IconKey;
    const FString AssetPath = FString::Printf(TEXT("/Game/UI/Icons/%s.png"), *IconKey);
    UTexture2D* Icon = LoadObject<UTexture2D>(nullptr, *AssetPath);
    if (Icon)
    {
        ItemIcon->SetBrushFromTexture(Icon);
        ItemIcon->SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        ItemIcon->SetVisibility(ESlateVisibility::Hidden);
    }

    const int32 Count = Item->GetCount();
    if (Count > 1)
    {
        ItemCountText->SetText(FText::AsNumber(Count));
        ItemCountText->SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        ItemCountText->SetVisibility(ESlateVisibility::Hidden);
    }
}

