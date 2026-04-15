#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Inventory/ClientInventoryTypes.h"
#include "ClientInventoryComponent.generated.h"

class UClientItemInstance;

// ChangedItem: 추가/수량 변경된 아이템. null이면 전체 목록이 바뀐 것 (제거, 초기화 등).
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FClientInventoryUpdatedSignature, UClientItemInstance*, ChangedItem);

UCLASS(BlueprintType)
class CLIENT_API UClientInventoryComponent : public UObject
{
    GENERATED_BODY()

public:
    UClientInventoryComponent();

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool AddItem(UClientItemInstance* Item);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool RemoveItemByKey(const FString& ItemKey);

    // 정렬된 아이템 목록을 반환한다.
    UFUNCTION(BlueprintPure, Category = "Inventory")
    const TArray<UClientItemInstance*>& GetItems() const;

    UFUNCTION(BlueprintPure, Category = "Inventory")
    UClientItemInstance* FindItemByKey(const FString& ItemKey) const;

    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FClientInventoryUpdatedSignature OnInventoryUpdated;

private:
    bool TryStackIntoExisting(UClientItemInstance* Item);
    void SortItems();
    void BroadcastItemUpdated(UClientItemInstance* ChangedItem);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
    TArray<TObjectPtr<UClientItemInstance>> Items;
};
