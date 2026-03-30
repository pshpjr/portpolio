#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/ClientUITypes.h"
#include "ClientPartyStatusWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FClientPartyStatusUpdatedSignature);

UCLASS(Blueprintable)
class CLIENT_API UClientPartyStatusWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Party")
    void SetPartyMembers(const TArray<FClientPartyMemberStatus>& InPartyMembers);

    UFUNCTION(BlueprintCallable, Category = "Party")
    bool UpsertPartyMember(const FClientPartyMemberStatus& InPartyMember);

    UFUNCTION(BlueprintCallable, Category = "Party")
    bool RemovePartyMember(const FString& MemberId);

    const TArray<FClientPartyMemberStatus>& GetPartyMembers() const;

    UPROPERTY(BlueprintAssignable, Category = "Party")
    FClientPartyStatusUpdatedSignature OnPartyStatusUpdated;

protected:
    int32 FindPartyMemberIndex(const FString& MemberId) const;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Party", meta = (AllowPrivateAccess = "true"))
    TArray<FClientPartyMemberStatus> PartyMembers;
};
