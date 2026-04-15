#include "UI/ClientConsentPromptWidget.h"

void UClientConsentPromptWidget::BroadcastPromptState()
{
    OnActivePromptChanged.Broadcast();
    OnConsentQueueChanged.Broadcast(PendingPrompts.Num());
}

int32 UClientConsentPromptWidget::FindPendingPromptIndex(const FString& PromptId) const
{
    if (PromptId.IsEmpty())
    {
        return INDEX_NONE;
    }

    for (int32 Index = 0; Index < PendingPrompts.Num(); ++Index)
    {
        if (PendingPrompts[Index].PromptId == PromptId)
        {
            return Index;
        }
    }

    return INDEX_NONE;
}

void UClientConsentPromptWidget::AdvanceToNextPrompt()
{
    if (PendingPrompts.IsEmpty())
    {
        bHasActivePrompt = false;
        ActivePrompt = FClientConsentPromptData();
        BroadcastPromptState();
        return;
    }

    ActivePrompt = PendingPrompts[0];
    PendingPrompts.RemoveAt(0);
    bHasActivePrompt = true;
    BroadcastPromptState();
}

bool UClientConsentPromptWidget::PushConsentPrompt(const FClientConsentPromptData& InPrompt)
{
    if (InPrompt.PromptId.IsEmpty())
    {
        return false;
    }

    if (bHasActivePrompt && ActivePrompt.PromptId == InPrompt.PromptId)
    {
        ActivePrompt = InPrompt;
        BroadcastPromptState();
        return true;
    }

    const int32 ExistingPendingIndex = FindPendingPromptIndex(InPrompt.PromptId);
    if (ExistingPendingIndex != INDEX_NONE)
    {
        PendingPrompts[ExistingPendingIndex] = InPrompt;
        OnConsentQueueChanged.Broadcast(PendingPrompts.Num());
        return true;
    }

    if (!bHasActivePrompt)
    {
        ActivePrompt = InPrompt;
        bHasActivePrompt = true;
        BroadcastPromptState();
        return true;
    }

    PendingPrompts.Add(InPrompt);
    OnConsentQueueChanged.Broadcast(PendingPrompts.Num());
    return true;
}

bool UClientConsentPromptWidget::RespondToActivePrompt(bool bAccepted)
{
    if (!bHasActivePrompt || ActivePrompt.PromptId.IsEmpty())
    {
        return false;
    }

    const FString PromptId = ActivePrompt.PromptId;
    OnConsentPromptResponded.Broadcast(PromptId, bAccepted);
    AdvanceToNextPrompt();
    return true;
}

void UClientConsentPromptWidget::ClearAllPrompts()
{
    bHasActivePrompt = false;
    ActivePrompt = FClientConsentPromptData();
    PendingPrompts.Reset();
    BroadcastPromptState();
}

bool UClientConsentPromptWidget::HasActivePrompt() const
{
    return bHasActivePrompt;
}

const FClientConsentPromptData& UClientConsentPromptWidget::GetActivePrompt() const
{
    return ActivePrompt;
}

const TArray<FClientConsentPromptData>& UClientConsentPromptWidget::GetPendingPrompts() const
{
    return PendingPrompts;
}
