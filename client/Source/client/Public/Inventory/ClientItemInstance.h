#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Generated/ItemTableRow.h"
#include "Inventory/ClientInventoryTypes.h"
#include "ClientItemInstance.generated.h"

UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class CLIENT_API UClientItemInstance : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void InitializeFromData(const FItemTableRow& InStaticData, const FClientItemRuntimeData& InRuntimeData);

    const FItemTableRow& GetStaticData() const;

    const FClientItemRuntimeData& GetRuntimeData() const;

    /** Blueprint용: 정적 데이터 전체 반환 (값 복사) */
    UFUNCTION(BlueprintPure, Category = "Inventory")
    FItemTableRow GetStaticDataBP() const { return StaticData; }

    /** Blueprint용: 런타임 데이터 전체 반환 (값 복사) */
    UFUNCTION(BlueprintPure, Category = "Inventory")
    FClientItemRuntimeData GetRuntimeDataBP() const { return RuntimeData; }

    UFUNCTION(BlueprintPure, Category = "Inventory")
    int32 GetItemId() const { return StaticData.ItemId; }

    UFUNCTION(BlueprintPure, Category = "Inventory")
    FString GetItemName() const { return StaticData.ItemName; }

    UFUNCTION(BlueprintPure, Category = "Inventory")
    int32 GetCount() const { return RuntimeData.Count; }

    UFUNCTION(BlueprintPure, Category = "Inventory")
    bool IsStackable() const;

    UFUNCTION(BlueprintPure, Category = "Inventory")
    bool IsBroken() const;

    UFUNCTION(BlueprintPure, Category = "Inventory")
    bool CanStackWith(const UClientItemInstance* OtherItem) const;

    UFUNCTION(BlueprintPure, Category = "Inventory")
    bool CanEquipInSlot(EEquipSlot TargetSlot) const;

    UFUNCTION(BlueprintPure, Category = "Inventory")
    bool CanMoveToStorage(EClientItemStorageKind TargetStorageKind) const;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void SetCount(int32 NewCount);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void SetDurability(int32 NewDurability);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void SetRemainingTradeCount(int32 NewTradeCount);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void SetAccountBound(bool bNewAccountBound);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void SetLocation(const FClientItemLocation& NewLocation);

private:
    FItemTableRow StaticData;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
    FClientItemRuntimeData RuntimeData;
};
