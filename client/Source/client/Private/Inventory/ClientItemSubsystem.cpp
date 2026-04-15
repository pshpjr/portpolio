#include "Inventory/ClientItemSubsystem.h"
#include "Inventory/ClientItemInstance.h"
#include "Misc/Paths.h"

void UClientItemSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    const FString DefaultPath = FPaths::ProjectContentDir() / TEXT("Data/item_table.data.json");
    if (!LoadFromFile(DefaultPath))
    {
        UE_LOG(LogTemp, Warning, TEXT("UClientItemSubsystem: 기본 경로에서 아이템 테이블 로드 실패: %s"), *DefaultPath);
    }
}

bool UClientItemSubsystem::LoadFromFile(const FString& FilePath)
{
    FString OutError;
    TArray<FItemTableRow> LoadedRows;

    if (!LoadItemTableFromFile(FilePath, LoadedRows, OutError))
    {
        UE_LOG(LogTemp, Warning, TEXT("UClientItemSubsystem: 로드 실패 (%s): %s"), *FilePath, *OutError);
        return false;
    }

    Rows = MoveTemp(LoadedRows);
    Index = BuildItemTableIndexByItemId(Rows);

    UE_LOG(LogTemp, Log, TEXT("UClientItemSubsystem: 아이템 %d개 로드 완료 (%s)"), Rows.Num(), *FilePath);
    return true;
}

const FItemTableRow* UClientItemSubsystem::FindItemRow(int32 ItemId) const
{
    return Index.Find(ItemId);
}

UClientItemInstance* UClientItemSubsystem::CreateItemInstance(int32 ItemId, const FClientItemRuntimeData& RuntimeData)
{
    const FItemTableRow* Row = FindItemRow(ItemId);
    if (!Row)
    {
        UE_LOG(LogTemp, Warning, TEXT("UClientItemSubsystem: ItemId=%d 를 찾을 수 없습니다."), ItemId);
        return nullptr;
    }

    UClientItemInstance* Instance = NewObject<UClientItemInstance>(GetGameInstance());
    Instance->InitializeFromData(*Row, RuntimeData);
    return Instance;
}
