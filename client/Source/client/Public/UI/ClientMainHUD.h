#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ClientMainHUD.generated.h"

class UPanelWidget;

/**
 * HUD에 등록할 패널 한 건의 메타데이터.
 * 레이아웃/렌더는 WidgetClass BP가 책임지고, 열림 규칙은 여기서 선언한다.
 */
USTRUCT(BlueprintType)
struct FClientHUDPanelDesc
{
    GENERATED_BODY()

    /** 패널 식별자. OpenPanel/ClosePanel에서 키로 사용. (예: "Inventory", "Storage", "Map") */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName PanelId;

    /** 이 패널을 그릴 UUserWidget 서브클래스. BP로 지정. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<UUserWidget> WidgetClass;

    /** 같은 그룹에 속한 패널들은 동시에 열리지 못한다. 비워두면 자유롭게 공존. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ExclusiveGroup;

    /** true면 열릴 때 다른 모든 패널을 닫고 혼자 남는다(모달 팝업 등). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bModal = false;

    /** ESC 닫기 스택에 포함할지. 팝업/모달은 true, 상시 HUD 요소는 false. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIncludeInCloseStack = true;

    /** 열릴 때 마우스 커서를 표시할지. PC가 이 힌트를 읽어 적용한다. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bRequiresMouseCursor = true;

    /**
     * 이 패널을 붙일 HUD 영역의 이름.
     * - 비워두거나 매칭되는 슬롯이 없으면 기본 PanelContainer로 들어간다.
     * - 표준 이름: TopLeft, TopCenter, TopRight, Left, Center, Right,
     *             BottomLeft, BottomCenter, BottomRight, Modal, Drag
     * - 그 외 임의 이름도 HUD의 자식 UPanelWidget 중 동일 이름이 있으면 자동 매칭된다.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName SlotName;

    /**
     * Overlay 계열 슬롯에 붙일 때의 정렬. 기본 Fill/Fill이라 패널이 슬롯 영역 안에 갇힌다.
     * 특정 코너에 작게 띄우고 싶으면 패널 루트를 SizeBox로 감싸거나 여기서 Left/Top 등으로 바꾼다.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TEnumAsByte<EHorizontalAlignment> HorizontalAlignment = HAlign_Fill;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TEnumAsByte<EVerticalAlignment> VerticalAlignment = VAlign_Fill;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FMargin Padding;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FClientHUDPanelVisibilityChanged, FName, PanelId, bool, bOpen);

/**
 * 스크린 루트 위젯. 인벤토리/창고/맵/파티창 등 여러 패널을 담고
 * 어떤 패널을 언제 여닫을지(배타, 모달, ESC 스택)를 관리한다.
 * 입력(IMC)은 PlayerController가 소유하고 여기에선 API만 호출한다.
 */
UCLASS(Abstract, Blueprintable)
class CLIENT_API UClientMainHUD : public UUserWidget
{
    GENERATED_BODY()

public:
    /** 패널 식별자로 열기. 존재하지 않으면 Registry에서 찾아 인스턴스를 지연 생성. */
    UFUNCTION(BlueprintCallable, Category = "HUD")
    UUserWidget* OpenPanel(FName PanelId);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ClosePanel(FName PanelId);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    UUserWidget* TogglePanel(FName PanelId);

    /** ESC 등으로 가장 최근에 열린 "닫기 가능" 패널을 하나 닫는다. 닫았으면 true. */
    UFUNCTION(BlueprintCallable, Category = "HUD")
    bool CloseTopmost();

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void CloseAll();

    UFUNCTION(BlueprintPure, Category = "HUD")
    bool IsPanelOpen(FName PanelId) const;

    UFUNCTION(BlueprintPure, Category = "HUD")
    UUserWidget* GetPanel(FName PanelId) const;

    /** 런타임 동적 등록. 에디터의 InitialPanels로 덮이지 않는 키만 추가한다. */
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void RegisterPanel(const FClientHUDPanelDesc& Desc);

    /** 현재 열린 패널 중 마우스 커서가 필요한 것이 하나라도 있는지. PC에서 커서 토글용으로 사용. */
    UFUNCTION(BlueprintPure, Category = "HUD")
    bool AnyOpenPanelNeedsCursor() const;

    UPROPERTY(BlueprintAssignable, Category = "HUD")
    FClientHUDPanelVisibilityChanged OnPanelVisibilityChanged;

protected:
    virtual void NativeConstruct() override;

    /** SlotName이 매칭되지 않는 패널이 들어가는 기본 컨테이너. */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UPanelWidget> PanelContainer = nullptr;

    /** 드래그 고스트/툴팁 오버레이용 최상단 레이어(선택). */
    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UPanelWidget> DragLayer = nullptr;

    // ── 표준 9-zone 네임드 슬롯 (모두 선택 바인딩) ─────────────────────────
    // 같은 이름으로 HUD BP에 UPanelWidget(Overlay 추천)을 두면 자동 인덱싱된다.
    UPROPERTY(meta = (BindWidgetOptional)) TObjectPtr<UPanelWidget> TopLeft = nullptr;
    UPROPERTY(meta = (BindWidgetOptional)) TObjectPtr<UPanelWidget> TopCenter = nullptr;
    UPROPERTY(meta = (BindWidgetOptional)) TObjectPtr<UPanelWidget> TopRight = nullptr;
    UPROPERTY(meta = (BindWidgetOptional)) TObjectPtr<UPanelWidget> Left = nullptr;
    UPROPERTY(meta = (BindWidgetOptional)) TObjectPtr<UPanelWidget> Center = nullptr;
    UPROPERTY(meta = (BindWidgetOptional)) TObjectPtr<UPanelWidget> Right = nullptr;
    UPROPERTY(meta = (BindWidgetOptional)) TObjectPtr<UPanelWidget> BottomLeft = nullptr;
    UPROPERTY(meta = (BindWidgetOptional)) TObjectPtr<UPanelWidget> BottomCenter = nullptr;
    UPROPERTY(meta = (BindWidgetOptional)) TObjectPtr<UPanelWidget> BottomRight = nullptr;

    /** 모든 일반 패널 위에 뜨는 모달 전용 레이어(선택). */
    UPROPERTY(meta = (BindWidgetOptional)) TObjectPtr<UPanelWidget> Modal = nullptr;

    /** 에디터에서 미리 등록할 패널 목록. */
    UPROPERTY(EditAnywhere, Category = "HUD")
    TArray<FClientHUDPanelDesc> InitialPanels;

    /** BP 훅. 열림/닫힘 이후 애니메이션·포커스 처리 등. */
    UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
    void OnPanelOpened(FName PanelId, UUserWidget* Widget);

    UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
    void OnPanelClosed(FName PanelId, UUserWidget* Widget);

private:
    UUserWidget* EnsurePanelInstance(const FClientHUDPanelDesc& Desc);
    void CloseExclusivesFor(const FClientHUDPanelDesc& Desc);
    void SetPanelVisibilityInternal(FName PanelId, UUserWidget* Widget, bool bOpen);

    /** BindWidget 슬롯 + 위젯트리 자동 스캔을 NamedSlots 맵에 적재. NativeConstruct에서 1회 호출. */
    void IndexNamedSlots();
    UPanelWidget* ResolveSlot(FName SlotName) const;

    UPROPERTY(Transient)
    TMap<FName, TObjectPtr<UPanelWidget>> NamedSlots;

    UPROPERTY(Transient)
    TMap<FName, FClientHUDPanelDesc> Registry;

    UPROPERTY(Transient)
    TMap<FName, TObjectPtr<UUserWidget>> Instances;

    /** 열린 순서대로 쌓이는 스택. ESC로 CloseTopmost 할 때 끝에서부터 본다. */
    UPROPERTY(Transient)
    TArray<FName> OpenStack;
};
