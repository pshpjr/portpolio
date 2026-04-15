#include "UI/ItemTooltipWidget.h"

#include "Inventory/ClientItemInstance.h"

void UItemTooltipWidget::SetItem(UClientItemInstance* InItem)
{
    Item = InItem;
    OnItemSet();
}
