#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ClientCurrencyComponent.generated.h"

/**
 * 통화 변경 알림. Delta는 NewAmount - OldAmount.
 * 전체 초기화 등으로 특정 키를 지목할 수 없을 때 CurrencyId=NAME_None으로 브로드캐스트한다.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
    FClientCurrencyChangedSignature,
    FName, CurrencyId, int64, NewAmount, int64, Delta);

/**
 * 캐릭터 도메인의 통화 보관소.
 * FName 키로 여러 통화(Gold/Gem/EventPoint 등)를 동시에 추적한다.
 * int32 대신 int64로 저장해 누적 상한(21억) 이슈를 피한다.
 */
UCLASS(BlueprintType)
class CLIENT_API UClientCurrencyComponent : public UObject
{
    GENERATED_BODY()

public:
    /** 단일 통화 잔액을 덮어쓴다. 서버 스냅샷 수신 시 주로 사용. */
    UFUNCTION(BlueprintCallable, Category = "Currency")
    void SetAmount(FName CurrencyId, int64 NewAmount);

    /** 현재 잔액에 Delta를 더한다(음수 가능). 음수 결과는 0으로 클램프. */
    UFUNCTION(BlueprintCallable, Category = "Currency")
    int64 AddAmount(FName CurrencyId, int64 Delta);

    UFUNCTION(BlueprintPure, Category = "Currency")
    int64 GetAmount(FName CurrencyId) const;

    UFUNCTION(BlueprintPure, Category = "Currency")
    bool HasEnough(FName CurrencyId, int64 RequiredAmount) const;

    /** 여러 통화를 한 번에 설정. 끝난 뒤 각 변경에 대해 개별 브로드캐스트. */
    UFUNCTION(BlueprintCallable, Category = "Currency")
    void SetBulk(const TMap<FName, int64>& NewAmounts);

    UPROPERTY(BlueprintAssignable, Category = "Currency")
    FClientCurrencyChangedSignature OnCurrencyChanged;

private:
    UPROPERTY(VisibleAnywhere, Category = "Currency")
    TMap<FName, int64> Balances;
};
