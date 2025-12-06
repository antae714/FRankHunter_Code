// Copyright F Rank Hunter. All Rights Reserved.


#include "UI/Lobby/FHSellSlot.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"

#include "Item/ItemTypes.h"
#include "Item/Data/FHItemClassDataAsset.h"
#include "BlueprintFunctions/FHBlueprintFunctionLibrary.h"

void UFHSellSlot::NativeConstruct()
{
	Super::NativeConstruct();

	DefaultBrush = ItemIcon->Background;
	SellCountSlot->SetVisibility(ESlateVisibility::Collapsed);
}

FReply UFHSellSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Result = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (bIsSelected && SellCountValue == 0)
	{
		OnOffSelected(false);
	}
	else if (!bIsSelected)
	{
		OnOffSelected(true);
	}

	return Result;
}

void UFHSellSlot::SetItemStack(int32 Stack)
{
	ItemStackValue = Stack;
	ItemStack->SetText(FText::FromString(FString::FromInt(ItemStackValue)));
}

void UFHSellSlot::ResetSlot()
{
	SellCountValue = 0;
	ItemStackValue = 0;
	bIsFullSelected = false;
	bIsSelected = false;
	bHasItem = false;
	ItemIcon->SetBrush(DefaultBrush);
	OnOffSelected(false);
}

void UFHSellSlot::OnOffSelected(bool On)
{
	FSlateBrush Brush = ItemIcon->Background;
	FSlateColor OutlineColor = Brush.OutlineSettings.Color;
	FLinearColor LinearColor = OutlineColor.GetSpecifiedColor();

	if (!On)
	{
		LinearColor.A = 0.0f;
		bIsSelected = false;
		SellCountSlot->SetVisibility(ESlateVisibility::Collapsed);
	}
	else if (On)
	{
		LinearColor.A = 1.0f;
		bIsSelected = true;
		SellCountSlot->SetVisibility(ESlateVisibility::Visible);
	}

	Brush.OutlineSettings.Color = LinearColor;
	ItemIcon->SetBrush(Brush);
}

void UFHSellSlot::OnDecreaseButtonClicked()
{
	if (SellCountValue == 0)
	{
		return;
	}

	SellCountValue = FMath::Max(SellCountValue - 1, 0);

	OnSellCountChanged();

	DecreaseSellPriceDelegate.ExecuteIfBound(ItemID);
}

void UFHSellSlot::OnIncreaseButtonClicked()
{
	if (SellCountValue == ItemStackValue)
	{
		return;
	}

	SellCountValue = FMath::Min(SellCountValue + 1, ItemStackValue);

	OnSellCountChanged();

	IncreaseSellPriceDelegate.ExecuteIfBound(ItemID);
}

void UFHSellSlot::SetItemIcon()
{
	UFHItemClassDataAsset* DataAsset = UFHBlueprintFunctionLibrary::GetItemClassDataAsset();
	TSoftObjectPtr<UDataTable> Table = UFHFRankHunterSettings::StaticClass()->GetDefaultObject<UFHFRankHunterSettings>()->ItemDataTable;
	check(DataAsset);

	FFHItemData* ItemData = Table->FindRow<FFHItemData>(ItemID, TEXT("ItemID"));
	check(ItemData);

	TSoftObjectPtr<UTexture2D> Texture = DataAsset->GetItemIcon(ItemData->IconID);
	FSlateBrush Brush = ItemIcon->Background;
	Brush.SetResourceObject(Texture.LoadSynchronous());
	ItemIcon->SetBrush(Brush);
}

void UFHSellSlot::OnSellCountChanged()
{
	SellCount->SetText(FText::FromString(FString::FromInt(SellCountValue)));

	if (SellCountValue == ItemStackValue)
	{
		FLinearColor BrushColor = ItemIcon->GetBrushColor();
		FLinearColor HSV = BrushColor.LinearRGBToHSV();
		HSV.B = 0.25f;
		FLinearColor NewColor = HSV.HSVToLinearRGB();
		NewColor.A = BrushColor.A;
		ItemIcon->SetBrushColor(NewColor);

		bIsFullSelected = true;
	}
	else if (SellCountValue < ItemStackValue && bIsFullSelected)
	{
		FLinearColor BrushColor = ItemIcon->GetBrushColor();
		FLinearColor HSV = BrushColor.LinearRGBToHSV();
		HSV.B = 1.0f;
		FLinearColor NewColor = HSV.HSVToLinearRGB();
		NewColor.A = BrushColor.A;
		ItemIcon->SetBrushColor(NewColor);

		bIsFullSelected = false;
	}
}

void UFHSellSlot::OnItemStackChanged()
{
	ItemStack->SetText(FText::FromString(FString::FromInt(ItemStackValue)));
}
