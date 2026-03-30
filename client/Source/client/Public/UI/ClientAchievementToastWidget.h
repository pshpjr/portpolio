#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/ClientUITypes.h"
#include "ClientAchievementToastWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FClientAchievementNotificationChangedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FClientAchievementQueueChangedSignature, int32, PendingNotificationCount);

UCLASS(Blueprintable)
class CLIENT_API UClientAchievementToastWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Achievement")
    bool PushAchievementNotification(const FClientAchievementNotification& InNotification);

    UFUNCTION(BlueprintCallable, Category = "Achievement")
    bool DismissActiveAchievement();

    UFUNCTION(BlueprintCallable, Category = "Achievement")
    void ClearNotifications();

    UFUNCTION(BlueprintPure, Category = "Achievement")
    bool HasActiveAchievement() const;

    const FClientAchievementNotification& GetActiveAchievement() const;
    const TArray<FClientAchievementNotification>& GetPendingNotifications() const;

    UPROPERTY(BlueprintAssignable, Category = "Achievement")
    FClientAchievementNotificationChangedSignature OnActiveAchievementChanged;

    UPROPERTY(BlueprintAssignable, Category = "Achievement")
    FClientAchievementQueueChangedSignature OnAchievementQueueChanged;

protected:
    void BroadcastNotificationState();
    void ActivateNextNotification();
    int32 FindPendingNotificationIndex(const FString& AchievementId) const;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Achievement", meta = (AllowPrivateAccess = "true"))
    bool bHasActiveAchievement = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Achievement", meta = (AllowPrivateAccess = "true"))
    FClientAchievementNotification ActiveAchievement;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Achievement", meta = (AllowPrivateAccess = "true"))
    TArray<FClientAchievementNotification> PendingNotifications;
};
