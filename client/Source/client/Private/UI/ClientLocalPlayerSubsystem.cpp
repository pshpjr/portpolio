#include "UI/ClientLocalPlayerSubsystem.h"

void UClientLocalPlayerSubsystem::OpenScreen(FName ScreenName)
{
    if (!OpenScreens.Contains(ScreenName))
    {
        OpenScreens.Add(ScreenName);
        OnScreenOpened.Broadcast(ScreenName);
    }
}

void UClientLocalPlayerSubsystem::CloseScreen(FName ScreenName)
{
    if (OpenScreens.Remove(ScreenName) > 0)
    {
        OnScreenClosed.Broadcast(ScreenName);
    }
}

bool UClientLocalPlayerSubsystem::IsScreenOpen(FName ScreenName) const
{
    return OpenScreens.Contains(ScreenName);
}

void UClientLocalPlayerSubsystem::BeginDrag(FName SourceContext, int32 SourceSlotIndex)
{
    DragState.bIsDragging = true;
    DragState.SourceContext = SourceContext;
    DragState.SourceSlotIndex = SourceSlotIndex;
}

void UClientLocalPlayerSubsystem::EndDrag()
{
    DragState = FClientDragState{};
}

void UClientLocalPlayerSubsystem::SetLocalSelection(FName Context, int32 SlotIndex)
{
    SelectedContext = Context;
    SelectedSlotIndex = SlotIndex;
}

void UClientLocalPlayerSubsystem::ClearLocalSelection()
{
    SelectedContext = NAME_None;
    SelectedSlotIndex = INDEX_NONE;
}

void UClientLocalPlayerSubsystem::SetTargetActor(AActor* InTarget)
{
    TargetActor = InTarget;
}

void UClientLocalPlayerSubsystem::PushNotification(const FText& Message)
{
    OnNotificationPushed.Broadcast(Message);
}

void UClientLocalPlayerSubsystem::SetActiveLayer(EClientUILayer Layer)
{
    ActiveLayer = Layer;
}
