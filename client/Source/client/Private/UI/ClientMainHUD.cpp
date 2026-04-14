#include "UI/ClientMainHUD.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/PanelWidget.h"

void UClientMainHUD::NativeConstruct()
{
    Super::NativeConstruct();

    IndexNamedSlots();

    // 에디터 지정 패널을 Registry에 복사. RegisterPanel과 동일 경로를 타도록 함.
    for (const FClientHUDPanelDesc& Desc : InitialPanels)
    {
        if (!Desc.PanelId.IsNone() && Desc.WidgetClass != nullptr)
        {
            Registry.Add(Desc.PanelId, Desc);
        }
    }
}

void UClientMainHUD::IndexNamedSlots()
{
    NamedSlots.Reset();

    // BindWidget으로 잡힌 표준 슬롯을 먼저 등록.
    auto Register = [this](FName Id, UPanelWidget* Widget)
    {
        if (Widget != nullptr)
        {
            NamedSlots.Add(Id, Widget);
        }
    };
    Register(TEXT("TopLeft"), TopLeft);
    Register(TEXT("TopCenter"), TopCenter);
    Register(TEXT("TopRight"), TopRight);
    Register(TEXT("Left"), Left);
    Register(TEXT("Center"), Center);
    Register(TEXT("Right"), Right);
    Register(TEXT("BottomLeft"), BottomLeft);
    Register(TEXT("BottomCenter"), BottomCenter);
    Register(TEXT("BottomRight"), BottomRight);
    Register(TEXT("Modal"), Modal);
    Register(TEXT("Drag"), DragLayer);

    // 위젯트리를 스캔해서 임의 이름의 UPanelWidget도 FName 그대로 등록.
    // 이미 BindWidget으로 등록한 것은 덮어쓰지 않는다.
    if (WidgetTree != nullptr)
    {
        WidgetTree->ForEachWidget([this](UWidget* Widget)
        {
            if (UPanelWidget* Panel = Cast<UPanelWidget>(Widget))
            {
                const FName Id = Panel->GetFName();
                if (!NamedSlots.Contains(Id))
                {
                    NamedSlots.Add(Id, Panel);
                }
            }
        });
    }
}

UPanelWidget* UClientMainHUD::ResolveSlot(FName SlotName) const
{
    if (SlotName.IsNone())
    {
        return PanelContainer;
    }
    if (const TObjectPtr<UPanelWidget>* Found = NamedSlots.Find(SlotName))
    {
        return Found->Get();
    }
    return PanelContainer;
}

void UClientMainHUD::RegisterPanel(const FClientHUDPanelDesc& Desc)
{
    if (Desc.PanelId.IsNone() || Desc.WidgetClass == nullptr)
    {
        return;
    }
    Registry.Add(Desc.PanelId, Desc);
}

UUserWidget* UClientMainHUD::OpenPanel(FName PanelId)
{
    const FClientHUDPanelDesc* Desc = Registry.Find(PanelId);
    if (Desc == nullptr || PanelContainer == nullptr)
    {
        return nullptr;
    }

    // 모달이면 전부 닫고, 배타 그룹이면 같은 그룹만 닫는다.
    if (Desc->bModal)
    {
        CloseAll();
    }
    else
    {
        CloseExclusivesFor(*Desc);
    }

    UUserWidget* Widget = EnsurePanelInstance(*Desc);
    if (Widget == nullptr)
    {
        return nullptr;
    }

    if (IsPanelOpen(PanelId))
    {
        // 이미 열려 있으면 스택 맨 뒤로 올려 "가장 최근에 열린" 상태로 만든다.
        OpenStack.Remove(PanelId);
        OpenStack.Add(PanelId);
        return Widget;
    }

    SetPanelVisibilityInternal(PanelId, Widget, true);
    return Widget;
}

void UClientMainHUD::ClosePanel(FName PanelId)
{
    TObjectPtr<UUserWidget>* Found = Instances.Find(PanelId);
    if (Found == nullptr || *Found == nullptr)
    {
        return;
    }
    if (!IsPanelOpen(PanelId))
    {
        return;
    }
    SetPanelVisibilityInternal(PanelId, *Found, false);
}

UUserWidget* UClientMainHUD::TogglePanel(FName PanelId)
{
    if (IsPanelOpen(PanelId))
    {
        ClosePanel(PanelId);
        return nullptr;
    }
    return OpenPanel(PanelId);
}

bool UClientMainHUD::CloseTopmost()
{
    for (int32 Index = OpenStack.Num() - 1; Index >= 0; --Index)
    {
        const FName PanelId = OpenStack[Index];
        const FClientHUDPanelDesc* Desc = Registry.Find(PanelId);
        if (Desc != nullptr && Desc->bIncludeInCloseStack)
        {
            ClosePanel(PanelId);
            return true;
        }
    }
    return false;
}

void UClientMainHUD::CloseAll()
{
    // 뒤에서부터 닫아야 스택 인덱스가 안전하게 소거된다.
    TArray<FName> Snapshot = OpenStack;
    for (int32 Index = Snapshot.Num() - 1; Index >= 0; --Index)
    {
        ClosePanel(Snapshot[Index]);
    }
}

bool UClientMainHUD::IsPanelOpen(FName PanelId) const
{
    return OpenStack.Contains(PanelId);
}

UUserWidget* UClientMainHUD::GetPanel(FName PanelId) const
{
    const TObjectPtr<UUserWidget>* Found = Instances.Find(PanelId);
    return Found ? Found->Get() : nullptr;
}

bool UClientMainHUD::AnyOpenPanelNeedsCursor() const
{
    for (const FName& PanelId : OpenStack)
    {
        const FClientHUDPanelDesc* Desc = Registry.Find(PanelId);
        if (Desc != nullptr && Desc->bRequiresMouseCursor)
        {
            return true;
        }
    }
    return false;
}

UUserWidget* UClientMainHUD::EnsurePanelInstance(const FClientHUDPanelDesc& Desc)
{
    if (TObjectPtr<UUserWidget>* Found = Instances.Find(Desc.PanelId))
    {
        return Found->Get();
    }

    UUserWidget* New = CreateWidget<UUserWidget>(this, Desc.WidgetClass);
    if (New == nullptr)
    {
        return nullptr;
    }

    // Desc.SlotName으로 대상 컨테이너 결정. 매칭 없으면 PanelContainer로 폴백.
    UPanelWidget* Target = ResolveSlot(Desc.SlotName);
    if (Target == nullptr)
    {
        return nullptr;
    }

    UPanelSlot* AddedSlot = Target->AddChild(New);

    // Overlay 슬롯 기본 정렬은 Left/Top이라 패널이 자기 DesiredSize만큼만 잡혀
    // 화면 밖으로 삐져나가기 쉽다. Desc 지정값을 적용해 기본 Fill/Fill이 되게 한다.
    if (UOverlaySlot* OS = Cast<UOverlaySlot>(AddedSlot))
    {
        OS->SetHorizontalAlignment(Desc.HorizontalAlignment);
        OS->SetVerticalAlignment(Desc.VerticalAlignment);
        OS->SetPadding(Desc.Padding);
    }

    New->SetVisibility(ESlateVisibility::Collapsed);
    Instances.Add(Desc.PanelId, New);
    return New;
}

void UClientMainHUD::CloseExclusivesFor(const FClientHUDPanelDesc& Desc)
{
    if (Desc.ExclusiveGroup.IsNone())
    {
        return;
    }
    TArray<FName> ToClose;
    for (const FName& OpenId : OpenStack)
    {
        const FClientHUDPanelDesc* Other = Registry.Find(OpenId);
        if (Other != nullptr && Other->ExclusiveGroup == Desc.ExclusiveGroup)
        {
            ToClose.Add(OpenId);
        }
    }
    for (const FName& Id : ToClose)
    {
        ClosePanel(Id);
    }
}

void UClientMainHUD::SetPanelVisibilityInternal(FName PanelId, UUserWidget* Widget, bool bOpen)
{
    if (Widget == nullptr)
    {
        return;
    }

    Widget->SetVisibility(bOpen ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

    if (bOpen)
    {
        OpenStack.Remove(PanelId);
        OpenStack.Add(PanelId);
        OnPanelOpened(PanelId, Widget);
    }
    else
    {
        OpenStack.Remove(PanelId);
        OnPanelClosed(PanelId, Widget);
    }

    OnPanelVisibilityChanged.Broadcast(PanelId, bOpen);
}
