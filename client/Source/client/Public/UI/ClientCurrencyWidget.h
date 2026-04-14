#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ClientCurrencyWidget.generated.h"

class UClientCurrencyComponent;
class UTextBlock;

/**
 * 특정 통화 하나를 바인딩해 텍스트로 표시한다.
 * BP에서 CurrencyId에 "Gold"/"Gem" 등 FName을 지정하면 해당 키만 구독.
 * 자동 포맷: 1,234,567 / 1.2K / 3.4M 등은 BP의 FormatAmount 훅에서 커스터마이즈 가능.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class CLIENT_API UClientCurrencyWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /** 표시할 통화 ID. 기본값은 Gold. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Currency")
    FName CurrencyId = TEXT("Gold");

    UFUNCTION(BlueprintCallable, Category = "Currency")
    void BindCurrencyComponent(UClientCurrencyComponent* InComponent);

    UFUNCTION(BlueprintPure, Category = "Currency")
    int64 GetDisplayedAmount() const { return DisplayedAmount; }

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    /** 표시용 텍스트 블록. BP에서 "AmountText" 이름으로 배치. */
    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UTextBlock> AmountText = nullptr;

    /** BP 포맷 훅. 기본은 자리수 콤마 포맷. BP에서 오버라이드하면 그 값이 우선. */
    UFUNCTION(BlueprintNativeEvent, Category = "Currency")
    FText FormatAmount(int64 Amount) const;
    virtual FText FormatAmount_Implementation(int64 Amount) const;

    /** 금액 변경 직후 BP 훅. 증감 표시/사운드/애니메이션용. */
    UFUNCTION(BlueprintImplementableEvent, Category = "Currency")
    void OnAmountChanged(int64 NewAmount, int64 Delta);

private:
    UFUNCTION()
    void HandleCurrencyChanged(FName InId, int64 NewAmount, int64 Delta);

    void RefreshText();

    UPROPERTY(Transient)
    TObjectPtr<UClientCurrencyComponent> BoundComponent = nullptr;

    UPROPERTY(Transient)
    int64 DisplayedAmount = 0;
};
