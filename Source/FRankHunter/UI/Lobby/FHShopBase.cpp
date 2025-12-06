// Copyright F Rank Hunter. All Rights Reserved.


#include "UI/Lobby/FHShopBase.h"
#include "FHBuyUIBase.h"
#include "FHSaleUIBase.h"
#include "FHSellCountBase.h"
#include "LobbyUIUtils.h"

#include "Components/Image.h"
#include "Item/ItemTypes.h"
#include "Item/Data/FHItemClassDataAsset.h"
#include "BlueprintFunctions/FHBlueprintFunctionLibrary.h"
#include "UI/Inventory/InventoryBase.h"
#include "Core/ManagerActorRegistrySubsystem.h"
#include "Core/FHPropManager.h"
#include "Props/FHTestCart.h"
#include "Item\FHInventoryComponent.h"

void UFHShopBase::NativeConstruct()
{
	Super::NativeConstruct();

	SaleUI->OnItemDroppedDelegate.BindDynamic(this, &UFHShopBase::OnItemDroppedOnSaleUI);
	if (CartInventoryUI->InventoryWeak == nullptr)
	{
		AFHPropManager* PropManager = GetWorld()->GetSubsystem<UManagerActorRegistrySubsystem>()->GetManagerActor<AFHPropManager>(TEXT("PropManager"));
		AFHTestCart* Storage = PropManager ? PropManager->GetProp<AFHTestCart>(TEXT("Cart")) : nullptr;
		if (Storage)
		{
			CartInventoryUI->SetInventoryComponent(Storage->GetComponentByClass<UFHInventoryComponent>());
		}
	}
}

void UFHShopBase::InitShopList()
{
	BuyUI->InitShopList();
}

void UFHShopBase::OnShopBuyCountChanged(const TArrayView<int32>& ChangedIndices, int32 FinalSize)
{
	BuyUI->OnShopBuyCountChanged(ChangedIndices, FinalSize);
}

void UFHShopBase::OnShopBuyCountChanged(const FName& ItemID, const int32 BuyCount)
{
	BuyUI->OnShopBuyCountChanged(ItemID, BuyCount);
}

void UFHShopBase::OnShopSellItemAdded(int32 InventoryIndex)
{
	SaleUI->OnSellItemAdded(InventoryIndex);
}

void UFHShopBase::OnShopSellItemRemoved(const TArray<int32>& Indices)
{
	SaleUI->OnSellItemRemoved(Indices);
}

void UFHShopBase::OnShopSellItemChanged(const TArray<int32>& Indices)
{
	SaleUI->OnSellItemChanged(Indices);
}

void UFHShopBase::OnItemDroppedOnSaleUI(FName ItemID, int32 ItemStack, int32 InventoryIndex, EItemMoveType MoveType)
{
	SellCount->ItemMoveType = MoveType;
	SellCount->ItemID = ItemID;
	SellCount->MaxItemStack = ItemStack;
	SellCount->InventoryIndex = InventoryIndex;
	SellCount->SellCountValue = ItemStack;
	SellCount->OnSellCountChanged();

	UFHItemClassDataAsset* DataAsset = UFHBlueprintFunctionLibrary::GetItemClassDataAsset();
	FFHItemData* ItemData = BuyUI->ItemTable->FindRow<FFHItemData>(ItemID, TEXT("ItemID"));
	if (DataAsset && ItemData)
	{
		SellCount->ItemIcon->SetBrushFromSoftTexture(DataAsset->GetItemIcon(ItemData->IconID));
	}
	SellCount->SetVisibility(ESlateVisibility::Visible);
}

void UFHShopBase::OpenUI_Implementation(const EUIType& UIType)
{
	if (UIType == EUIType::Buy)
	{
		BuyUI->OpenUI();
		BuyUI->SetVisibility(ESlateVisibility::Visible);
		SaleUI->SetVisibility(ESlateVisibility::Collapsed);
	}
	else if (UIType == EUIType::Sale)
	{
		SaleUI->OpenUI();
		BuyUI->SetVisibility(ESlateVisibility::Collapsed);
		SaleUI->SetVisibility(ESlateVisibility::Visible);
	}
}
