// Copyright F Rank Hunter. All Rights Reserved.


#include "FHItemListBase.h"
#include "FHBuyUIBase.h"
#include "FHSaleUIBase.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "LobbyUIUtils.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "UI/Inventory/InventoryItemBase.h"
#include "UI/common/ItemDrag.h"
#include "Item/ItemTypes.h"
#include "Item/Data/FHItemClassDataAsset.h"
#include "BlueprintFunctions/FHBlueprintFunctionLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Core/FHFRankHunterSettings.h"

void UFHItemListBase::SetBorder(bool Active)
{
	FSlateBrush Brush = Border->Background;
	FSlateColor OutlineColor = Brush.OutlineSettings.Color;
	FLinearColor LinearColor = OutlineColor.GetSpecifiedColor();

	if (Active)
	{
		LinearColor.A = 1.0f;
	}
	else
	{
		LinearColor.A = 0.0f;
	}

	OutlineColor = LinearColor;
	Brush.OutlineSettings.Color = OutlineColor;

	Border->SetBrush(Brush);
}

void UFHItemListBase::NativeConstruct()
{
	Super::NativeConstruct();

	if (Count == 0)
	{
		SetBorder(false);
	}
}

void UFHItemListBase::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	//if (OutOperation == nullptr)
	//{
	//	if (bIsBuyListUI)
	//	{
	//		return;
	//	}

	//	UItemDragImage* ItemDrag = Cast<UItemDragImage>(UWidgetBlueprintLibrary::CreateDragDropOperation(UItemDragImage::StaticClass()));

	//	if (ItemPayload == nullptr)
	//	{
	//		ItemPayload = CreateWidget<UItemDragImagePayloadBase>(this, DragPayloadClass);
	//		ItemPayload->OwnerWidget = this;

	//		UFHFRankHunterSettings* Settings = UFHFRankHunterSettings::StaticClass()->GetDefaultObject<UFHFRankHunterSettings>();
	//		FFHItemData* ItemTableRow = nullptr;
	//		if (Settings)
	//		{
	//			ItemTableRow = Settings->ItemDataTable->FindRow<FFHItemData>(ItemID, TEXT("ItemID"));
	//		}

	//		UFHItemClassDataAsset* DataAsset = UFHBlueprintFunctionLibrary::GetItemClassDataAsset();
	//		if (DataAsset && ItemTableRow)
	//		{
	//			TSoftObjectPtr<UTexture2D> SoftTex = DataAsset->GetItemIcon(ItemTableRow->IconID);
	//			ItemPayload->SetItemIcon(SoftTex);
	//		}
	//	}

	//	ItemDrag->DefaultDragVisual = ItemPayload;
	//	ItemDrag->Pivot = EDragPivot::MouseDown;
	//	ItemDrag->ItemID = ItemID;
	//	ItemDrag->ItemStack = ItemStackValue;
	//	ItemDrag->OnItemDroppedDelegatePtr = &SaleUI->OnItemDroppedDelegate;
	//	OutOperation = ItemDrag;
	//}
}

FReply UFHItemListBase::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Result = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	//if (!Result.IsEventHandled())
	//{
	//	Result = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	//}

	return Result;
}

void UFHItemListBase::InitLayOut_Implementation(const EUIType& UIType)
{

}

void UFHItemListBase::OnCountSubButtonClicked()
{
	if (Count == 0)
	{
		return;
	}

	Count = FMath::Max(0, Count - 1);
	OnCountValueChanged();
	BuyUI->RemoveFromShoppingCart(ItemID, Cost);

	if (Count == 0)
	{
		SetBorder(false);
	}
}

void UFHItemListBase::OnCountAddButtonClicked()
{
	if (Count == MaxBuyCount)
	{
		return;
	}

	Count = FMath::Min(MaxBuyCount, Count + 1);
	OnCountValueChanged();
	BuyUI->AddToShoppingCart(ItemID, Cost);

	if (Count > 0)
	{
		SetBorder(true);
	}
}

void UFHItemListBase::OnCountValueChanged()
{
	BuyCount->SetText(FText::FromString(*FString::FromInt(Count)));

	if (Count == 0)
	{
		SetBorder(false);
	}
}

void UFHItemListBase::OnMaxBuyCountChanged()
{
	CanBuyCount->SetText(FText::FromString(*FString::FromInt(MaxBuyCount)));
	if (MaxBuyCount == 0)
	{
		SetSoldOut();
	}
}

void UFHItemListBase::ResetCount()
{
	Count = 0;
	OnCountValueChanged();
	//BuyUI->RemoveFromShoppingCart(ItemID, Cost);
}
