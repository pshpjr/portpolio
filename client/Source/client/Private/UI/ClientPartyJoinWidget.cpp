#include "UI/ClientPartyJoinWidget.h"

void UClientPartyJoinWidget::SetPartyListings(const TArray<FClientPartyJoinListing>& InPartyListings)
{
    PartyListings = InPartyListings;
    OnPartyListingsChanged.Broadcast();
}

void UClientPartyJoinWidget::ClearPartyListings()
{
    PartyListings.Reset();
    OnPartyListingsChanged.Broadcast();
}

bool UClientPartyJoinWidget::RequestJoinParty(const FString& PartyId)
{
    if (PartyId.IsEmpty())
    {
        return false;
    }

    for (const FClientPartyJoinListing& PartyListing : PartyListings)
    {
        if (PartyListing.PartyId == PartyId)
        {
            OnPartyJoinRequested.Broadcast(PartyId);
            return true;
        }
    }

    return false;
}

const TArray<FClientPartyJoinListing>& UClientPartyJoinWidget::GetPartyListings() const
{
    return PartyListings;
}
