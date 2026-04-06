// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Inventory/ClientConsumableQuickSlotComponent.h"
#include "Inventory/ClientEquipmentComponent.h"
#include "Inventory/ClientInventoryComponent.h"
#include "Inventory/ClientSkillHotbarComponent.h"
#include "Inventory/ClientStorageComponent.h"
#include "ClientPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class CLIENT_API AClientPlayerController : public APlayerController
{
	
	GENERATED_BODY()

public:
	AClientPlayerController();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	UClientInventoryComponent* Inventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	UClientEquipmentComponent* Equipment;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	UClientSkillHotbarComponent* SkillHotbar;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	UClientStorageComponent* Storage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	UClientConsumableQuickSlotComponent* ConsumableQuickSlot;
};
