#include "Inventory/ClientItemSubsystem.h"
#include "Inventory/ClientItemInstance.h"
#include "Misc/Paths.h"

void UClientItemSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    UE_LOG(LogTemp, Log, TEXT("UClientItemSubsystem: Initialize 시작"));
    UE_LOG(LogTemp, Log, TEXT("UClientItemSubsystem: ProjectDir=%s"), *FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()));
    UE_LOG(LogTemp, Log, TEXT("UClientItemSubsystem: ini DataDirectory='%s'"), *DataDirectory);

    FString ResolvedDir = DataDirectory;
    if (ResolvedDir.IsEmpty())
    {
        ResolvedDir = FPaths::ProjectContentDir() / TEXT("Data");
        UE_LOG(LogTemp, Log, TEXT("UClientItemSubsystem: DataDirectory 비어있음 → 기본값(Content/Data) 사용"));
    }
    else if (FPaths::IsRelative(ResolvedDir))
    {
        const FString Before = ResolvedDir;
        ResolvedDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir(), ResolvedDir);
        UE_LOG(LogTemp, Log, TEXT("UClientItemSubsystem: 상대경로 '%s' → 절대경로 '%s'"), *Before, *ResolvedDir);
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("UClientItemSubsystem: 절대경로 그대로 사용"));
    }

    const bool bDirExists = FPaths::DirectoryExists(ResolvedDir);
    UE_LOG(LogTemp, Log, TEXT("UClientItemSubsystem: 최종 ResolvedDir=%s (존재=%s)"),
        *ResolvedDir, bDirExists ? TEXT("true") : TEXT("false"));

    const FString PrimaryPath = ResolvedDir / TEXT("item_table.data.json");
    UE_LOG(LogTemp, Log, TEXT("UClientItemSubsystem: 1차 시도 경로=%s"), *PrimaryPath);

    if (LoadFromFile(PrimaryPath))
    {
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("UClientItemSubsystem: 설정 경로에서 아이템 테이블 로드 실패: %s"), *PrimaryPath);

    const FString FallbackPath = FPaths::ProjectContentDir() / TEXT("Data/item_table.data.json");
    if (FallbackPath == PrimaryPath)
    {
        UE_LOG(LogTemp, Warning, TEXT("UClientItemSubsystem: 폴백 경로가 1차 경로와 동일 → 추가 시도 없음"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("UClientItemSubsystem: 폴백 경로 시도=%s"), *FallbackPath);
    if (LoadFromFile(FallbackPath))
    {
        UE_LOG(LogTemp, Log, TEXT("UClientItemSubsystem: 폴백 경로에서 아이템 테이블 로드 성공: %s"), *FallbackPath);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UClientItemSubsystem: 모든 경로에서 아이템 테이블 로드 실패"));
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
