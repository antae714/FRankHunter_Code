// Copyright F Rank Hunter. All Rights Reserved.


#include "UI/Foundation/FHTabButton.h"
#include "CommonLazyImage.h"

void UFHTabButton::SetTabDescriptor_Implementation(const FFHTabDescriptor& TabDescriptor)
{
	SetButtonText(TabDescriptor.TabText);

	SetIconBrush(TabDescriptor.IconBrush);
}

void UFHTabButton::SetIconFromLazyObject(TSoftObjectPtr<UObject> LazyObject)
{
	if (LazyImageIcon)
	{
		LazyImageIcon->SetBrushFromLazyDisplayAsset(LazyObject);
	}
}

void UFHTabButton::SetIconBrush(const FSlateBrush& Brush)
{
	if (LazyImageIcon)
	{
		LazyImageIcon->SetBrush(Brush);
	}
}
