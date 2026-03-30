#include "UI/ClientPartyMemberEntryWidget.h"

void UClientPartyMemberEntryWidget::SetPartyMemberData(const FClientPartyMemberStatus& InPartyMemberData)
{
    PartyMemberData = InPartyMemberData;
    OnPartyMemberDataChanged.Broadcast();
}

const FClientPartyMemberStatus& UClientPartyMemberEntryWidget::GetPartyMemberData() const
{
    return PartyMemberData;
}
