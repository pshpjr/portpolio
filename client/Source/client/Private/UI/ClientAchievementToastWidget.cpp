#include "UI/ClientAchievementToastWidget.h"

void UClientAchievementToastWidget::BroadcastNotificationState()
{
    OnActiveAchievementChanged.Broadcast();
    OnAchievementQueueChanged.Broadcast(PendingNotifications.Num());
}

void UClientAchievementToastWidget::ActivateNextNotification()
{
    if (PendingNotifications.IsEmpty())
    {
        bHasActiveAchievement = false;
        ActiveAchievement = FClientAchievementNotification();
        BroadcastNotificationState();
        return;
    }

    ActiveAchievement = PendingNotifications[0];
    PendingNotifications.RemoveAt(0);
    bHasActiveAchievement = true;
    BroadcastNotificationState();
}

int32 UClientAchievementToastWidget::FindPendingNotificationIndex(const FString& AchievementId) const
{
    if (AchievementId.IsEmpty())
    {
        return INDEX_NONE;
    }

    for (int32 Index = 0; Index < PendingNotifications.Num(); ++Index)
    {
        if (PendingNotifications[Index].AchievementId == AchievementId)
        {
            return Index;
        }
    }

    return INDEX_NONE;
}

bool UClientAchievementToastWidget::PushAchievementNotification(const FClientAchievementNotification& InNotification)
{
    if (InNotification.AchievementId.IsEmpty())
    {
        return false;
    }

    if (bHasActiveAchievement && ActiveAchievement.AchievementId == InNotification.AchievementId)
    {
        ActiveAchievement = InNotification;
        BroadcastNotificationState();
        return true;
    }

    const int32 ExistingPendingIndex = FindPendingNotificationIndex(InNotification.AchievementId);
    if (ExistingPendingIndex != INDEX_NONE)
    {
        PendingNotifications[ExistingPendingIndex] = InNotification;
        OnAchievementQueueChanged.Broadcast(PendingNotifications.Num());
        return true;
    }

    if (!bHasActiveAchievement)
    {
        ActiveAchievement = InNotification;
        bHasActiveAchievement = true;
        BroadcastNotificationState();
        return true;
    }

    PendingNotifications.Add(InNotification);
    OnAchievementQueueChanged.Broadcast(PendingNotifications.Num());
    return true;
}

bool UClientAchievementToastWidget::DismissActiveAchievement()
{
    if (!bHasActiveAchievement || ActiveAchievement.AchievementId.IsEmpty())
    {
        return false;
    }

    ActivateNextNotification();
    return true;
}

void UClientAchievementToastWidget::ClearNotifications()
{
    bHasActiveAchievement = false;
    ActiveAchievement = FClientAchievementNotification();
    PendingNotifications.Reset();
    BroadcastNotificationState();
}

bool UClientAchievementToastWidget::HasActiveAchievement() const
{
    return bHasActiveAchievement;
}

const FClientAchievementNotification& UClientAchievementToastWidget::GetActiveAchievement() const
{
    return ActiveAchievement;
}

const TArray<FClientAchievementNotification>& UClientAchievementToastWidget::GetPendingNotifications() const
{
    return PendingNotifications;
}
