#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "ClientUITypes.generated.h"

UENUM(BlueprintType)
enum class EClientPartyJoinPolicy : uint8
{
    Public,
    InviteOnly
};

UENUM(BlueprintType)
enum class EClientConsentPromptType : uint8
{
    Generic,
    PartyInvite,
    DungeonEntry,
    MatchReady,
    TradeRequest
};

USTRUCT(BlueprintType)
struct CLIENT_API FClientPartyJoinListing
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party")
    FString PartyId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party")
    FText PartyTitle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party")
    FText LeaderName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party")
    FText ActivityLabel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party")
    int32 CurrentMemberCount = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party")
    int32 MaxMemberCount = 4;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party")
    int32 RecommendedLevel = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party")
    EClientPartyJoinPolicy JoinPolicy = EClientPartyJoinPolicy::Public;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party")
    bool bIsInDungeon = false;
};

USTRUCT(BlueprintType)
struct CLIENT_API FClientPartyMemberStatus
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party")
    FString MemberId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party")
    FText CharacterName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party")
    TSoftObjectPtr<UTexture2D> PortraitTexture;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party")
    int32 Level = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party")
    float CurrentHealth = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party")
    float MaxHealth = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party")
    bool bIsLeader = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party")
    bool bIsConnected = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Party")
    bool bIsLocalPlayer = false;
};

USTRUCT(BlueprintType)
struct CLIENT_API FClientConsentPromptData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consent")
    FString PromptId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consent")
    EClientConsentPromptType PromptType = EClientConsentPromptType::Generic;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consent")
    FText Title;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consent")
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consent")
    FText ConfirmLabel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consent")
    FText CancelLabel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consent")
    float TimeoutSeconds = 30.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consent")
    bool bAllowDecline = true;
};

USTRUCT(BlueprintType)
struct CLIENT_API FClientAchievementNotification
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
    FString AchievementId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
    FText Title;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
    FText Subtitle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
    FText RewardSummary;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
    TSoftObjectPtr<UTexture2D> IconTexture;
};
