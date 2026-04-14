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

    /** 현재 열려 있는 슬롯 수. Items.Num()이 이 값에 도달하면 AddItem이 실패한다. */
    UFUNCTION(BlueprintPure, Category = "Inventory")
    int32 GetCurrentCapacity() const { return CurrentCapacity; }

    /** 확장 상한. UI는 항상 이 값만큼 슬롯 위젯을 그리고 CurrentCapacity 초과분은 잠금 표시. */
    UFUNCTION(BlueprintPure, Category = "Inventory")
    int32 GetMaxCapacity() const { return GClientInventoryMaxCapacity; }

    /** 서버가 준 값으로 현재 용량을 설정한다. [0, Max]로 클램프되고 OnInventoryUpdated(null)이 브로드캐스트된다. */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void SetCurrentCapacity(int32 NewCapacity);

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

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
    int32 CurrentCapacity = GClientInventoryDefaultCapacity;
};
