// Copyright F Rank Hunter. All Rights Reserved.


#include "UI/ItemQuickSlot/FHItemQuickSlotElement.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Item/FHItemBase.h"
#include "BluePrintFunctions\FHBlueprintFunctionLibrary.h"
#include "Item/Data/FHItemClassDataAsset.h"

void UFHItemQuickSlotElement::NativeConstruct()
{
	Super::NativeConstruct();

	FocusImage->SetVisibility(ESlateVisibility::Hidden);
	SetFocus(false);
}

void UFHItemQuickSlotElement::SetFocus(bool bIsFocus)
{
	if (bIsFocus)
	{
		FocusImage->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		FocusImage->SetVisibility(ESlateVisibility::Hidden);
	}
}
