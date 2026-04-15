#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ClientCharacterSubsystem.generated.h"

class UClientInventoryComponent;
class UClientEquipmentComponent;
class UClientSkillHotbarComponent;
class UClientStorageComponent;
class UClientConsumableQuickSlotComponent;

/**
 * GameInstanceSubsystem — 캐릭터 도메인 원본 캐시.
 *
 * 인벤토리, 장비, 스킬 핫바, 보관함, 소모품 퀵슬롯 데이터를 보유한다.
 * 서버 패킷 수신 시 각 도메인 객체에 직접 반영한다.
 *
 * 접근 방법:
 *   GetGameInstance()->GetSubsystem<UClientCharacterSubsystem>()
 */
UCLASS()
class CLIENT_API UClientCharacterSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintPure, Category = "Character")
    UClientInventoryComponent* GetInventory() const { return Inventory; }

    UFUNCTION(BlueprintPure, Category = "Character")
    UClientEquipmentComponent* GetEquipment() const { return Equipment; }

    UFUNCTION(BlueprintPure, Category = "Character")
    UClientSkillHotbarComponent* GetSkillHotbar() const { return SkillHotbar; }

    UFUNCTION(BlueprintPure, Category = "Character")
    UClientStorageComponent* GetStorage() const { return Storage; }

    UFUNCTION(BlueprintPure, Category = "Character")
    UClientConsumableQuickSlotComponent* GetConsumableQuickSlot() const { return ConsumableQuickSlot; }

private:
    UPROPERTY()
    TObjectPtr<UClientInventoryComponent> Inventory;

    UPROPERTY()
    TObjectPtr<UClientEquipmentComponent> Equipment;

    UPROPERTY()
    TObjectPtr<UClientSkillHotbarComponent> SkillHotbar;

    UPROPERTY()
    TObjectPtr<UClientStorageComponent> Storage;

    UPROPERTY()
    TObjectPtr<UClientConsumableQuickSlotComponent> ConsumableQuickSlot;
};
