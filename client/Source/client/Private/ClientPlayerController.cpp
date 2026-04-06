// Fill out your copyright notice in the Description page of Project Settings.


#include "ClientPlayerController.h"

AClientPlayerController::AClientPlayerController()
{
	Inventory = CreateDefaultSubobject<UClientInventoryComponent>(TEXT("Inventory"));
	Equipment = CreateDefaultSubobject<UClientEquipmentComponent>(TEXT("Equipment"));
	SkillHotbar = CreateDefaultSubobject<UClientSkillHotbarComponent>(TEXT("SkillHotbar"));
	Storage = CreateDefaultSubobject<UClientStorageComponent>(TEXT("Storage"));
	ConsumableQuickSlot = CreateDefaultSubobject<UClientConsumableQuickSlotComponent>(TEXT("ConsumableQuickSlot"));
}
