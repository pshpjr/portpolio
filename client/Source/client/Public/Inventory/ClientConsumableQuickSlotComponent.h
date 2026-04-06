#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/ClientInventoryTypes.h"
#include "ClientConsumableQuickSlotComponent.generated.h"

class UClientItemInstance;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FClientConsumableQuickSlotUpdatedSignature);

// 기획 규칙:
// - 슬롯 2개 (Slot1, Slot2), 슬롯당 최대 5개 스택
// - 쿨다운은 슬롯 단위 (아이템 단위 아님)
// - 던전 내부에서는 슬롯 구성(아이템 종류) 변경 불가 — 이 컴포넌트는 상태만 보관, 제한 로직은 호출부 책임
UCLASS(ClassGroup = (Custom), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class CLIENT_API UClientConsumableQuickSlotComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UClientConsumableQuickSlotComponent();

    // 슬롯에 소모품 아이템을 설정한다. 이미 다른 아이템이 있으면 교체한다.
    UFUNCTION(BlueprintCallable, Category = "Consumable")
    bool AssignConsumable(UClientItemInstance* Item, EClientConsumableSlot Slot);

    // 슬롯의 소모품을 제거하고 반환한다.
    UFUNCTION(BlueprintCallable, Category = "Consumable")
    UClientItemInstance* ClearSlot(EClientConsumableSlot Slot);

    // 슬롯의 소모품을 1개 소모한다. 수량이 0이 되면 슬롯을 비운다.
    UFUNCTION(BlueprintCallable, Category = "Consumable")
    bool ConsumeFromSlot(EClientConsumableSlot Slot);

    UFUNCTION(BlueprintPure, Category = "Consumable")
    UClientItemInstance* GetItemAtSlot(EClientConsumableSlot Slot) const;

    UFUNCTION(BlueprintPure, Category = "Consumable")
    int32 GetCountAtSlot(EClientConsumableSlot Slot) const;

    UPROPERTY(BlueprintAssignable, Category = "Consumable")
    FClientConsumableQuickSlotUpdatedSignature OnConsumableQuickSlotUpdated;

private:
    bool IsValidSlot(EClientConsumableSlot Slot) const;
    void BroadcastUpdated();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Consumable", meta = (AllowPrivateAccess = "true"))
    TMap<EClientConsumableSlot, TObjectPtr<UClientItemInstance>> SlotItems;
};
