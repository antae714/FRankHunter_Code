// Copyright F Rank Hunter. All Rights Reserved.


#include "UI/Lobby/FHSaleUIBase.h"
#include "UI/Common/ItemDrag.h"
#include "UI/Inventory/InventoryItemBase.h"
#include "Item/FHItemBase.h"
#include "FHItemListBase.h"
#include "LobbyUIUtils.h"

#include "Item/ItemTypes.h"
#include "Item/Data/FHItemClassDataAsset.h"
#include "BlueprintFunctions/FHBlueprintFunctionLibrary.h"

#include "Kismet/GameplayStatics.h"
#include "Props/FHKioskBase.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"

#include "Core/FHPlayerController.h"

bool UFHSaleUIBase::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bool Result = Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	if (!Result)
	{
		UItemDrag* ItemDrag = Cast<UItemDrag>(InOperation);
		if (ItemDrag)
		{
			if (ItemDrag->WidgetReference->CachedItemInstance == nullptr)
			{
				return Result;
			}
			int32 ItemStack = ItemDrag->WidgetReference->CachedStack;
			FName ItemID = ItemDrag->WidgetReference->CachedItemInstance->GetItemData().ItemID;
			int32 ItemIndex = ItemDrag->WidgetReference->CachedItemIndex;

			OnItemDroppedDelegate.ExecuteIfBound(ItemID, ItemStack, ItemIndex, EItemMoveType::InventoryToShop);
		}
	}
	return Result;
}

void UFHSaleUIBase::OnSellItemAdded(int32 InventoryIndex)
{
	//if (!ItemListWidgetClass)
	//{
	//	return;
	//}

	//AFHKioskBase* Kiosk = Cast<AFHKioskBase>(UGameplayStatics::GetActorOfClass(GetWorld(), AFHKioskBase::StaticClass()));
	//UFHItemClassDataAsset* DataAsset = UFHBlueprintFunctionLibrary::GetItemClassDataAsset();
	//if (Kiosk && DataAsset)
	//{
	//	check(Kiosk->SellingItemList.Items.Num() > InventoryIndex);

	//	FName ItemID = Kiosk->SellingItemList.Items[InventoryIndex].ItemID;
	//	int32 ItemStack = Kiosk->SellingItemList.Items[InventoryIndex].ItemStack;

	//	FFHItemData* ItemData = ItemTable->FindRow<FFHItemData>(ItemID, TEXT("ItemID"));
	//	if (ItemData)
	//	{
	//		UFHItemListBase* ItemListUI = nullptr;
	//		for (UFHItemListBase* CachedItemListUI : CachedItemList)
	//		{
	//			if (CachedItemListUI->ItemID == ItemID)
	//			{
	//				ItemListUI = CachedItemListUI;
	//			}
	//		}

	//		if (ItemListUI == nullptr)
	//		{
	//			ItemListUI = CreateWidget<UFHItemListBase>(this, ItemListWidgetClass);
	//			if (ItemListUI)
	//			{
	//				ItemListUI->InitLayOut(EUIType::Sale);
	//				ItemListUI->SaleUI = this;
	//				ItemListUI->ItemID = ItemData->ItemID;
	//				ItemListUI->ItemIcon->SetBrushFromSoftTexture(DataAsset->GetItemIcon(ItemData->IconID));
	//				ItemListUI->ItemName->SetText(DataAsset->GetItemName(ItemData->ItemNameID));
	//				ItemListUI->ItemDescription->SetText(DataAsset->GetItemDescription(ItemData->DescriptionID));
	//				ItemListUI->Price->SetText(FText::FromString(FString::FromInt(ItemData->SellPrice)));
	//				ItemListUI->Cost = ItemData->SellPrice;
	//				ItemListUI->ItemStackValue = ItemStack;
	//				ItemListUI->ItemStack->SetText(FText::FromString(FString::FromInt(ItemStack)));


	//				FLinearColor ItemRarityColor = UFHBlueprintFunctionLibrary::GetItemRarityColor(ItemData->Rarity);
	//				ItemListUI->ItemName->SetColorAndOpacity(ItemRarityColor);

	//				TotalPrice += ItemData->SellPrice * ItemStack;
	//				OnTotalPriceChanged();

	//				CachedItemList.Add(ItemListUI);
	//				ItemScrollList->AddChild(ItemListUI);
	//			}
	//		}
	//	}
	//}
}

void UFHSaleUIBase::OnSellItemRemoved(const TArray<int32>& Indices)
{
	for (int32 Index : Indices)
	{
		UFHItemListBase* ItemListUI = CachedItemList[Index];
		TotalPrice -= ItemListUI->Cost * ItemListUI->ItemStackValue;
		OnTotalPriceChanged();

		ItemScrollList->RemoveChildAt(Index);
		CachedItemList.RemoveAt(Index);
	}
}

void UFHSaleUIBase::OnSellItemChanged(const TArray<int32>& Indices)
{
	//AFHKioskBase* Kiosk = Cast<AFHKioskBase>(UGameplayStatics::GetActorOfClass(GetWorld(), AFHKioskBase::StaticClass()));
	//if (Kiosk)
	//{
	//	for (int32 Index : Indices)
	//	{
	//		UFHItemListBase* ItemListUI = CachedItemList[Index];
	//		int32 ItemStack = Kiosk->SellingItemList.Items[Index].ItemStack;
	//		ItemListUI->ItemStack->SetText(FText::FromString(FString::FromInt(ItemStack)));
	//		int32 IncreasedStack = ItemStack - ItemListUI->ItemStackValue;

	//		ItemListUI->ItemStackValue = ItemStack;
	//		TotalPrice += ItemListUI->Cost * IncreasedStack;
	//		OnTotalPriceChanged();
	//	}
	//}
}

void UFHSaleUIBase::OpenUI()
{
	OpenPlayerCount++;
}

void UFHSaleUIBase::CloseUI()
{
	OpenPlayerCount--;
	if (OpenPlayerCount == 0)
	{
		if (CachedItemList.IsEmpty())
		{
			return;
		}
		// TODO1: PlayerController->RemoveSellList();
		// TODO2: Shop->SellStateEnd();
	}
}

void UFHSaleUIBase::OnSaleSuccess()
{

}

void UFHSaleUIBase::OnTotalPriceChanged()
{
	TotalPayment->SetText(FText::FromString(FString::FromInt(TotalPrice)));
}

void UFHSaleUIBase::OnSaleButtonClicked()
{
	AFHPlayerController* Controller = Cast<AFHPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (Controller)
	{
		//Controller->SellItem(TotalPrice);
	}
}
