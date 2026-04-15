#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/ClientUITypes.h"
#include "ClientPartyMemberEntryWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FClientPartyMemberDataChangedSignature);

UCLASS(Blueprintable)
class CLIENT_API UClientPartyMemberEntryWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Party")
    void SetPartyMemberData(const FClientPartyMemberStatus& InPartyMemberData);

    const FClientPartyMemberStatus& GetPartyMemberData() const;

    UPROPERTY(BlueprintAssignable, Category = "Party")
    FClientPartyMemberDataChangedSignature OnPartyMemberDataChanged;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Party", meta = (AllowPrivateAccess = "true"))
    FClientPartyMemberStatus PartyMemberData;
};
