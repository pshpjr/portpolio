#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/ClientUITypes.h"
#include "ClientConsentPromptWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FClientConsentPromptChangedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FClientConsentQueueChangedSignature, int32, PendingPromptCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FClientConsentPromptRespondedSignature, FString, PromptId, bool, bAccepted);

UCLASS(Blueprintable)
class CLIENT_API UClientConsentPromptWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Consent")
    bool PushConsentPrompt(const FClientConsentPromptData& InPrompt);

    UFUNCTION(BlueprintCallable, Category = "Consent")
    bool RespondToActivePrompt(bool bAccepted);

    UFUNCTION(BlueprintCallable, Category = "Consent")
    void ClearAllPrompts();

    UFUNCTION(BlueprintPure, Category = "Consent")
    bool HasActivePrompt() const;

    const FClientConsentPromptData& GetActivePrompt() const;
    const TArray<FClientConsentPromptData>& GetPendingPrompts() const;

    UPROPERTY(BlueprintAssignable, Category = "Consent")
    FClientConsentPromptChangedSignature OnActivePromptChanged;

    UPROPERTY(BlueprintAssignable, Category = "Consent")
    FClientConsentQueueChangedSignature OnConsentQueueChanged;

    UPROPERTY(BlueprintAssignable, Category = "Consent")
    FClientConsentPromptRespondedSignature OnConsentPromptResponded;

protected:
    void BroadcastPromptState();
    int32 FindPendingPromptIndex(const FString& PromptId) const;
    void AdvanceToNextPrompt();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Consent", meta = (AllowPrivateAccess = "true"))
    bool bHasActivePrompt = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Consent", meta = (AllowPrivateAccess = "true"))
    FClientConsentPromptData ActivePrompt;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Consent", meta = (AllowPrivateAccess = "true"))
    TArray<FClientConsentPromptData> PendingPrompts;
};
