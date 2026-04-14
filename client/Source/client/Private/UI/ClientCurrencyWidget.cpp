#include "UI/ClientCurrencyWidget.h"

#include "ClientCharacterSubsystem.h"
#include "Components/TextBlock.h"
#include "Inventory/ClientCurrencyComponent.h"
#include "Internationalization/FastDecimalFormat.h"

void UClientCurrencyWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (BoundComponent == nullptr)
    {
        if (UGameInstance* GI = GetGameInstance())
        {
            if (UClientCharacterSubsystem* CharSub = GI->GetSubsystem<UClientCharacterSubsystem>())
            {
                BindCurrencyComponent(CharSub->GetCurrency());
            }
        }
    }
    else
    {
        RefreshText();
    }
}

void UClientCurrencyWidget::NativeDestruct()
{
    if (BoundComponent != nullptr)
    {
        BoundComponent->OnCurrencyChanged.RemoveDynamic(this, &UClientCurrencyWidget::HandleCurrencyChanged);
        BoundComponent = nullptr;
    }
    Super::NativeDestruct();
}

void UClientCurrencyWidget::BindCurrencyComponent(UClientCurrencyComponent* InComponent)
{
    if (BoundComponent == InComponent)
    {
        RefreshText();
        return;
    }

    if (BoundComponent != nullptr)
    {
        BoundComponent->OnCurrencyChanged.RemoveDynamic(this, &UClientCurrencyWidget::HandleCurrencyChanged);
    }

    BoundComponent = InComponent;

    if (BoundComponent != nullptr)
    {
        BoundComponent->OnCurrencyChanged.AddDynamic(this, &UClientCurrencyWidget::HandleCurrencyChanged);
    }

    RefreshText();
}

FText UClientCurrencyWidget::FormatAmount_Implementation(int64 Amount) const
{
    // 기본: 1,234,567 형식. 통화 기호는 BP에서 앞뒤에 붙이도록 둔다.
    FNumberFormattingOptions Options;
    Options.SetUseGrouping(true);
    return FText::AsNumber(Amount, &Options);
}

void UClientCurrencyWidget::HandleCurrencyChanged(FName InId, int64 NewAmount, int64 Delta)
{
    // 관심 없는 통화 키는 무시. NAME_None 브로드캐스트(전체 리셋)는 모두 갱신한다.
    if (!InId.IsNone() && InId != CurrencyId)
    {
        return;
    }
    RefreshText();
    if (InId == CurrencyId)
    {
        OnAmountChanged(NewAmount, Delta);
    }
}

void UClientCurrencyWidget::RefreshText()
{
    DisplayedAmount = BoundComponent != nullptr ? BoundComponent->GetAmount(CurrencyId) : 0;
    if (AmountText != nullptr)
    {
        AmountText->SetText(FormatAmount(DisplayedAmount));
    }
}
