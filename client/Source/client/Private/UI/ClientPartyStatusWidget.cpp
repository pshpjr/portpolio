#include "UI/ClientPartyStatusWidget.h"

void UClientPartyStatusWidget::SetPartyMembers(const TArray<FClientPartyMemberStatus>& InPartyMembers)
{
    PartyMembers = InPartyMembers;
    OnPartyStatusUpdated.Broadcast();
}

bool UClientPartyStatusWidget::UpsertPartyMember(const FClientPartyMemberStatus& InPartyMember)
{
    if (InPartyMember.MemberId.IsEmpty())
    {
        return false;
    }

    const int32 ExistingIndex = FindPartyMemberIndex(InPartyMember.MemberId);
    if (ExistingIndex != INDEX_NONE)
    {
        PartyMembers[ExistingIndex] = InPartyMember;
    }
    else
    {
        PartyMembers.Add(InPartyMember);
    }

    OnPartyStatusUpdated.Broadcast();
    return true;
}

bool UClientPartyStatusWidget::RemovePartyMember(const FString& MemberId)
{
    const int32 ExistingIndex = FindPartyMemberIndex(MemberId);
    if (ExistingIndex == INDEX_NONE)
    {
        return false;
    }

    PartyMembers.RemoveAt(ExistingIndex);
    OnPartyStatusUpdated.Broadcast();
    return true;
}

const TArray<FClientPartyMemberStatus>& UClientPartyStatusWidget::GetPartyMembers() const
{
    return PartyMembers;
}

int32 UClientPartyStatusWidget::FindPartyMemberIndex(const FString& MemberId) const
{
    if (MemberId.IsEmpty())
    {
        return INDEX_NONE;
    }

    for (int32 Index = 0; Index < PartyMembers.Num(); ++Index)
    {
        if (PartyMembers[Index].MemberId == MemberId)
        {
            return Index;
        }
    }

    return INDEX_NONE;
}
