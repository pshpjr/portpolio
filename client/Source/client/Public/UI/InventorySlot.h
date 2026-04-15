// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Inventory/ClientItemInstance.h"
#include "InventorySlot.generated.h"

class UItemTooltipWidget;

/**
 * 
 */
UCLASS()
class CLIENT_API UInventorySlot : public UUserWidget
{
	GENERATED_BODY()
	
	
public:
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetItem(UClientItemInstance* InItem);

	/** 이 슬롯이 유효한 아이템 데이터를 들고 있는지. 빈 슬롯/미초기화 인스턴스면 false. */
	UFUNCTION(BlueprintPure, Category = "UI")
	bool HasItem() const { return IsValid(Item) && Item->HasValidData(); }

	/** 슬롯을 빈 상태로 리셋한다. 내부적으로 SetItem(nullptr)과 동일. */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void Clear() { SetItem(nullptr); }

	/** 슬롯을 잠금/해제 상태로 전환한다. 잠금이면 아이템을 비우고 LockedOverlay를 표시한다. */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetLocked(bool bInLocked);

	UFUNCTION(BlueprintPure, Category = "UI")
	bool IsLocked() const { return bLocked; }

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	TObjectPtr<UClientItemInstance> Item = nullptr;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> ItemIcon = nullptr;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> ItemCountText = nullptr;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UBorder> SelectedBorder = nullptr;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> RedDotImage = nullptr;
	
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> EnhanceText = nullptr;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UImage> EquippedMark = nullptr;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UImage> DisabledOverlay = nullptr;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UBorder> DragHighlightBorder = nullptr;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UImage> LockedOverlay = nullptr;

	/** 슬롯 픽셀 크기를 전달받아 내부 텍스트 폰트를 자동 스케일. 컨테이너에서 호출. */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ApplyPixelSize(FVector2D InPixelSize);

	/** 호버 시 띄울 툴팁 위젯 클래스. 비어 있거나 아이템이 없으면 툴팁 없음. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UItemTooltipWidget> TooltipClass;

protected:
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;

	/** 폰트 스케일 계수. 개수/강화 텍스트 각각 슬롯 높이에 곱해 사용. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout|Font")
	float CountFontScale = 0.22f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout|Font")
	float EnhanceFontScale = 0.18f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout|Font", meta = (ClampMin = "6"))
	int32 CountFontMin = 9;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout|Font", meta = (ClampMin = "6"))
	int32 EnhanceFontMin = 8;

	/** 아이콘 패딩을 슬롯 크기 비례로 계산할 때의 계수. 0이면 패딩 없음. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout|Icon", meta = (ClampMin = "0.0"))
	float IconPaddingScale = 0.08f;

private:
	/** ItemIcon이 OverlaySlot에 있으면 Fill 정렬로 고정하고 패딩을 슬롯 크기 비례로 세팅. */
	void NormalizeItemIconSlot();

	/** ToolTipWidgetDelegate에 바인드되는 함수. 아이템이 있을 때만 툴팁 인스턴스를 생성해 반환. */
	UFUNCTION()
	UWidget* BuildTooltipWidget();

	/** 현재 캐시된 슬롯 픽셀 크기(마지막 ApplyPixelSize 결과). 0이면 64로 가정. */
	UPROPERTY(Transient)
	FVector2D CachedPixelSize = FVector2D(64.f, 64.f);


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	bool bLocked = false;
};
