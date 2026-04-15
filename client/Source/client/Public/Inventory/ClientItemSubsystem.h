#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Generated/ItemTableRow.h"
#include "Inventory/ClientInventoryTypes.h"
#include "ClientItemSubsystem.generated.h"

class UClientItemInstance;

/**
 * GameInstanceSubsystem — JSON 기반 아이템 테이블 보유 + 인스턴스 팩토리.
 *
 * 서버 패킷 수신 흐름:
 *   1. 패킷에서 ItemId(int32)와 런타임 데이터(FClientItemRuntimeData)를 추출한다.
 *   2. GetSubsystem<UClientItemSubsystem>()->CreateItemInstance(ItemId, RuntimeData) 호출.
 *   3. 반환된 UClientItemInstance를 인벤토리 컴포넌트에 전달한다.
 *
 * 데이터 로딩:
 *   Initialize() 에서 DataDirectory/item_table.data.json 을 자동으로 로드한다.
 *   DataDirectory 는 DefaultGame.ini 의 [/Script/client.ClientItemSubsystem] 섹션에서
 *   지정하며, 상대 경로는 프로젝트 디렉터리 기준으로 해석된다.
 *   설정이 없거나 로드 실패 시 Content/Data 로 폴백한다.
 *   다른 경로를 사용하려면 LoadFromFile()을 직접 호출한다.
 */
UCLASS(Config=Game)
class CLIENT_API UClientItemSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // UGameInstanceSubsystem
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    /**
     * 데이터 JSON 파일이 있는 디렉터리.
     * 절대 경로 또는 ProjectDir 기준 상대 경로.
     * DefaultGame.ini 의 DataDirectory= 값에서 주입된다.
     */
    UPROPERTY(Config, EditAnywhere, Category = "Item")
    FString DataDirectory;

    /**
     * JSON 파일에서 아이템 테이블을 로드한다.
     * Initialize()에서 자동 호출되므로, 경로 재지정이 필요할 때만 직접 호출한다.
     */
    UFUNCTION(BlueprintCallable, Category = "Item")
    bool LoadFromFile(const FString& FilePath);

    /**
     * ItemId로 테이블 행을 조회해 UClientItemInstance를 생성한다.
     * Outer는 내부적으로 GameInstance를 사용하므로 호출자가 지정할 필요 없다.
     * 데이터 미로드 또는 ItemId 없으면 nullptr 반환 + 경고 로그.
     */
    UFUNCTION(BlueprintCallable, Category = "Item")
    UClientItemInstance* CreateItemInstance(int32 ItemId, const FClientItemRuntimeData& RuntimeData);

    /**
     * ItemId → FItemTableRow 포인터 반환.
     * 테이블에 없으면 nullptr.
     */
    const FItemTableRow* FindItemRow(int32 ItemId) const;

    UFUNCTION(BlueprintPure, Category = "Item")
    bool IsDataLoaded() const { return Index.Num() > 0; }

private:
    TArray<FItemTableRow> Rows;
    TMap<int32, FItemTableRow> Index;
};
