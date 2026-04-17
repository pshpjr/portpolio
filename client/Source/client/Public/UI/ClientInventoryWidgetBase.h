#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Layout/Margin.h"
#include "Generated/game_data/enums.h"
#include "ClientInventoryWidgetBase.generated.h"

class UClientInventoryComponent;
class UClientItemInstance;
class UInventorySlot;
class UUniformGridPanel;

UCLASS(Abstract, BlueprintType, Blueprintable)
class CLIENT_API UClientInventoryWidgetBase : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "UI")
    void BindInventoryComponent(UClientInventoryComponent* InInventoryComponent);

    UFUNCTION(BlueprintPure, Category = "UI")
    UClientInventoryComponent* GetInventoryComponent() const;

    /** 슬롯 위젯 클래스. BP에서 지정. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout")
    TSubclassOf<UInventorySlot> SlotClass;

    /** 한 행에 들어갈 슬롯 수. 에디터에서 조정 가능. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout", meta = (ClampMin = "1"))
    int32 NumColumns = 8;

    /** 슬롯 간 패딩. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout")
    FMargin SlotPadding = FMargin(2.f);

    /** 각 슬롯의 고정 크기(px). 0 이하면 override 미적용(슬롯 위젯 자체 DesiredSize 사용). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout")
    FVector2D SlotSize = FVector2D(64.f, 64.f);

    /**
     * 탭 필터. None이면 전체 표시. 특정 카테고리일 땐 그 카테고리가 아닌 슬롯은 빈칸으로 표시한다.
     * 슬롯 인덱스/순서는 그대로 유지되므로 드래그·서버 동기화 시 혼선이 없다.
     */
    UFUNCTION(BlueprintCallable, Category = "UI")
    void SetActiveCategory(EItemCategory NewCategory);

    UFUNCTION(BlueprintPure, Category = "UI")
    EItemCategory GetActiveCategory() const { return ActiveCategory; }

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual void NativePreConstruct() override;

    /** 선택적 훅: 기본 갱신이 끝난 뒤 BP가 후처리할 수 있음 (애니메이션, 포커스 이동 등). */
    UFUNCTION(BlueprintImplementableEvent, Category = "UI")
    void OnInventoryUpdated(UClientItemInstance* ChangedItem);

    /** 레이아웃 파라미터 변경/기동 시 `MaxCapacity`만큼 슬롯을 생성해 `SlotContainer`에 채운다. */
    void RebuildSlots();

    /** 현재 용량/잠금 상태와 아이템 목록을 슬롯 위젯에 반영한다. */
    void RefreshSlots();

    /** BP가 이 이름으로 UUniformGridPanel을 배치해 두면 슬롯이 채워진다. */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UUniformGridPanel> SlotContainer = nullptr;

private:
    UFUNCTION()
    void HandleInventoryUpdated(UClientItemInstance* ChangedItem);

    UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UClientInventoryComponent> InventoryComponent = nullptr;

    UPROPERTY(Transient)
    TArray<TObjectPtr<UInventorySlot>> SlotWidgets;

    UPROPERTY(EditAnywhere, Category = "UI", meta = (AllowPrivateAccess = "true"))
    EItemCategory ActiveCategory = EItemCategory::None;
};
