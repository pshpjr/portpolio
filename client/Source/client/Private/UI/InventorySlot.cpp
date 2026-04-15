// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/InventorySlot.h"

#include "Components/OverlaySlot.h"
#include "UI/ItemTooltipWidget.h"

void UInventorySlot::NativePreConstruct()
{
    Super::NativePreConstruct();
    // Designer 프리뷰에서도 캐시된 크기로 폰트/아이콘 패딩을 스케일.
    ApplyPixelSize(CachedPixelSize);
}

void UInventorySlot::NativeConstruct()
{
    Super::NativeConstruct();
    ApplyPixelSize(CachedPixelSize);

    // 호버 툴팁 바인딩. 기존 바인딩이 있어도 덮어써서 최신 TooltipClass가 적용되도록 함.
    ToolTipWidgetDelegate.BindDynamic(this, &UInventorySlot::BuildTooltipWidget);
}

UWidget* UInventorySlot::BuildTooltipWidget()
{
    if (!HasItem() || !TooltipClass)
    {
        return nullptr;
    }

    UItemTooltipWidget* Tooltip = CreateWidget<UItemTooltipWidget>(this, TooltipClass);
    if (Tooltip != nullptr)
    {
        Tooltip->SetItem(Item);
    }
    return Tooltip;
}

void UInventorySlot::ApplyPixelSize(FVector2D InPixelSize)
{
    CachedPixelSize = InPixelSize;
    const float H = InPixelSize.Y > 0.f ? InPixelSize.Y : 64.f;

    // 아이콘 정렬/패딩을 슬롯 크기 비례로 갱신.
    NormalizeItemIconSlot();

    if (ItemCountText)
    {
        FSlateFontInfo Font = ItemCountText->GetFont();
        Font.Size = FMath::Clamp(FMath::RoundToInt(H * CountFontScale), CountFontMin, 48);
        Font.OutlineSettings.OutlineSize = 1;
        Font.OutlineSettings.OutlineColor = FLinearColor::Black;
        ItemCountText->SetFont(Font);
        ItemCountText->SetShadowOffset(FVector2D(1.f, 1.f));
        ItemCountText->SetShadowColorAndOpacity(FLinearColor(0.f, 0.f, 0.f, 0.5f));
    }

    if (EnhanceText)
    {
        FSlateFontInfo Font = EnhanceText->GetFont();
        Font.Size = FMath::Clamp(FMath::RoundToInt(H * EnhanceFontScale), EnhanceFontMin, 40);
        Font.OutlineSettings.OutlineSize = 1;
        Font.OutlineSettings.OutlineColor = FLinearColor::Black;
        EnhanceText->SetFont(Font);
    }
}

void UInventorySlot::NormalizeItemIconSlot()
{
    if (ItemIcon == nullptr)
    {
        return;
    }
    if (UOverlaySlot* OS = Cast<UOverlaySlot>(ItemIcon->Slot))
    {
        OS->SetHorizontalAlignment(HAlign_Fill);
        OS->SetVerticalAlignment(VAlign_Fill);

        const float Ref = FMath::Min(
            CachedPixelSize.X > 0.f ? CachedPixelSize.X : 64.f,
            CachedPixelSize.Y > 0.f ? CachedPixelSize.Y : 64.f);
        const float Pad = FMath::Max(0.f, Ref * IconPaddingScale);
        OS->SetPadding(FMargin(Pad));
    }
}

void UInventorySlot::SetItem(UClientItemInstance* InItem)
{
    // 잠긴 슬롯엔 아이템을 넣지 않는다. 잠금 해제 후 다시 호출할 것.
    if (bLocked)
    {
        Item = nullptr;
        return;
    }

    Item = InItem;

    if (Item == nullptr)
    {
        ItemIcon->SetVisibility(ESlateVisibility::Hidden);
        ItemCountText->SetVisibility(ESlateVisibility::Hidden);
        if (SelectedBorder)
        {
            SelectedBorder->SetVisibility(ESlateVisibility::Hidden);
        }
        if (RedDotImage)
        {
            RedDotImage->SetVisibility(ESlateVisibility::Hidden);
        }
        if (EnhanceText)
        {
            EnhanceText->SetVisibility(ESlateVisibility::Hidden);
        }
        if (EquippedMark)
        {
            EquippedMark->SetVisibility(ESlateVisibility::Hidden);
        }
        if (DisabledOverlay)
        {
            DisabledOverlay->SetVisibility(ESlateVisibility::Hidden);
        }
        if (DragHighlightBorder)
        {
            DragHighlightBorder->SetVisibility(ESlateVisibility::Hidden);
        }
        return;
    }

    const FItemTableRow& StaticData = Item->GetStaticData();
    const FString& IconKey = StaticData.IconKey;
    const FString CategoryStr = StaticEnum<EItemCategory>()->GetNameStringByValue(
        static_cast<int64>(StaticData.ItemCategory));
    const FString AssetPath = FString::Printf(
        TEXT("/Game/UI/Icons/%s/T_%s.T_%s"), *CategoryStr, *IconKey, *IconKey);
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

void UInventorySlot::SetLocked(bool bInLocked)
{
    bLocked = bInLocked;

    if (bLocked)
    {
        // 잠금 시엔 아이템/보조 비주얼을 모두 비우고 LockedOverlay만 표시.
        Item = nullptr;
        ItemIcon->SetVisibility(ESlateVisibility::Hidden);
        ItemCountText->SetVisibility(ESlateVisibility::Hidden);
        if (SelectedBorder)     { SelectedBorder->SetVisibility(ESlateVisibility::Hidden); }
        if (RedDotImage)        { RedDotImage->SetVisibility(ESlateVisibility::Hidden); }
        if (EnhanceText)        { EnhanceText->SetVisibility(ESlateVisibility::Hidden); }
        if (EquippedMark)       { EquippedMark->SetVisibility(ESlateVisibility::Hidden); }
        if (DisabledOverlay)    { DisabledOverlay->SetVisibility(ESlateVisibility::Hidden); }
        if (DragHighlightBorder){ DragHighlightBorder->SetVisibility(ESlateVisibility::Hidden); }
        if (LockedOverlay)      { LockedOverlay->SetVisibility(ESlateVisibility::Visible); }
    }
    else
    {
        if (LockedOverlay)      { LockedOverlay->SetVisibility(ESlateVisibility::Hidden); }
        // 잠금 해제 시엔 빈 슬롯 상태로 리셋. 아이템은 호출 측이 다시 SetItem으로 넣는다.
        SetItem(nullptr);
    }
}

