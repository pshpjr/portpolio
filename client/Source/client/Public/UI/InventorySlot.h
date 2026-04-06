// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Inventory/ClientItemInstance.h"
#include "InventorySlot.generated.h"

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
};
