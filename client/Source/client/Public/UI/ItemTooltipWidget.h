#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemTooltipWidget.generated.h"

class UClientItemInstance;

/**
 * 인벤토리/창고 슬롯 호버 시 띄우는 아이템 정보 툴팁의 베이스.
 * 실제 레이아웃(이름/설명/아이콘/스탯 등)은 이 클래스를 상속한 BP에서 구성하고,
 * `OnItemSet`에서 `Item` 프로퍼티를 읽어 각 요소를 갱신한다.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class CLIENT_API UItemTooltipWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /** 슬롯이 호버될 때 호출. Item을 세팅하고 BP `OnItemSet`을 트리거한다. */
    UFUNCTION(BlueprintCallable, Category = "UI")
    void SetItem(UClientItemInstance* InItem);

    UFUNCTION(BlueprintPure, Category = "UI")
    UClientItemInstance* GetItem() const { return Item; }

protected:
    /** BP 훅. Item이 세팅된 직후 호출되며 여기서 텍스트/아이콘/스탯을 채운다. */
    UFUNCTION(BlueprintImplementableEvent, Category = "UI")
    void OnItemSet();

    UPROPERTY(BlueprintReadOnly, Category = "UI")
    TObjectPtr<UClientItemInstance> Item = nullptr;
};
