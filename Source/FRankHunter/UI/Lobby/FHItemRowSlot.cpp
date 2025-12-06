// Copyright F Rank Hunter. All Rights Reserved.


#include "UI/Lobby/FHItemRowSlot.h"
#include "Components/HorizontalBox.h"
#include "FHSellSlot.h"


const TArray<UWidget*> UFHItemRowSlot::GetChildArray()
{
	return ItemRow->GetAllChildren();
}

UWidget* UFHItemRowSlot::GetItemSlot(int32 ItemIndex)
{
	return ItemRow->GetChildAt(ItemIndex);
}