#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Inventory/ClientInventoryTypes.h"
#include "ClientItemInstance.generated.h"

UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class CLIENT_API UClientItemInstance : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void InitializeFromData(const FClientItemStaticData& InStaticData, const FClientItemRuntimeData& InRuntimeData);

    const FClientItemStaticData& GetStaticData() const;

    const FClientItemRuntimeData& GetRuntimeData() const;

    UFUNCTION(BlueprintPure, Category = "Inventory")
    bool IsStackable() const;

    UFUNCTION(BlueprintPure, Category = "Inventory")
    bool IsBroken() const;

    UFUNCTION(BlueprintPure, Category = "Inventory")
    bool CanStackWith(const UClientItemInstance* OtherItem) const;

    UFUNCTION(BlueprintPure, Category = "Inventory")
    bool CanEquipInSlot(EClientEquipSlot TargetSlot) const;

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
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
    FClientItemStaticData StaticData;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
    FClientItemRuntimeData RuntimeData;
};
