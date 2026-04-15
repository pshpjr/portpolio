#include "Inventory/ClientCurrencyComponent.h"

void UClientCurrencyComponent::SetAmount(FName CurrencyId, int64 NewAmount)
{
    if (CurrencyId.IsNone())
    {
        return;
    }

    const int64 Clamped = FMath::Max<int64>(0, NewAmount);
    const int64 Old = Balances.FindRef(CurrencyId);
    if (Old == Clamped)
    {
        return;
    }
    Balances.Add(CurrencyId, Clamped);
    OnCurrencyChanged.Broadcast(CurrencyId, Clamped, Clamped - Old);
}

int64 UClientCurrencyComponent::AddAmount(FName CurrencyId, int64 Delta)
{
    if (CurrencyId.IsNone() || Delta == 0)
    {
        return GetAmount(CurrencyId);
    }
    const int64 Old = Balances.FindRef(CurrencyId);
    const int64 NewAmount = FMath::Max<int64>(0, Old + Delta);
    Balances.Add(CurrencyId, NewAmount);
    OnCurrencyChanged.Broadcast(CurrencyId, NewAmount, NewAmount - Old);
    return NewAmount;
}

int64 UClientCurrencyComponent::GetAmount(FName CurrencyId) const
{
    return Balances.FindRef(CurrencyId);
}

bool UClientCurrencyComponent::HasEnough(FName CurrencyId, int64 RequiredAmount) const
{
    return GetAmount(CurrencyId) >= RequiredAmount;
}

void UClientCurrencyComponent::SetBulk(const TMap<FName, int64>& NewAmounts)
{
    for (const TPair<FName, int64>& Pair : NewAmounts)
    {
        SetAmount(Pair.Key, Pair.Value);
    }
}
