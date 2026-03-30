#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/ClientUITypes.h"
#include "ClientPartyJoinWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FClientPartyJoinListingsChangedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FClientPartyJoinRequestedSignature, FString, PartyId);

UCLASS(Blueprintable)
class CLIENT_API UClientPartyJoinWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Party")
    void SetPartyListings(const TArray<FClientPartyJoinListing>& InPartyListings);

    UFUNCTION(BlueprintCallable, Category = "Party")
    void ClearPartyListings();

    UFUNCTION(BlueprintCallable, Category = "Party")
    bool RequestJoinParty(const FString& PartyId);

    const TArray<FClientPartyJoinListing>& GetPartyListings() const;

    UPROPERTY(BlueprintAssignable, Category = "Party")
    FClientPartyJoinListingsChangedSignature OnPartyListingsChanged;

    UPROPERTY(BlueprintAssignable, Category = "Party")
    FClientPartyJoinRequestedSignature OnPartyJoinRequested;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Party", meta = (AllowPrivateAccess = "true"))
    TArray<FClientPartyJoinListing> PartyListings;
};
