// Copyright F Rank Hunter. All Rights Reserved.


#include "UI/Lobby/FHSellCountBase.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Core/FHPlayerController.h"
#include "LobbyUIUtils.h"

#include "Kismet/GameplayStatics.h"

void UFHSellCountBase::OnIncreaseButtonClicked()
{
	SellCountValue++;
	SellCountValue = FMath::Min(SellCountValue, MaxItemStack);
	OnSellCountChanged();
}

void UFHSellCountBase::OnDecreaseButtonClicked()
{
	SellCountValue--;
	SellCountValue = FMath::Max(0, SellCountValue);
	OnSellCountChanged();
}

void UFHSellCountBase::OnOkButtonClicked()
{
	AFHPlayerController* Controller = Cast<AFHPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (Controller)
	{
		if (ItemMoveType == EItemMoveType::InventoryToShop)
		{
			FSellItemInfo ItemInfo;
			ItemInfo.ItemID = ItemID;
			ItemInfo.InventoryIndex = InventoryIndex;
			ItemInfo.ItemStack = SellCountValue;
			Controller->AddToSellItemCart(ItemInfo);
		}
		else if (ItemMoveType == EItemMoveType::ShopToInventory)
		{
			FSellItemInfo ItemInfo;
			ItemInfo.ItemID = ItemID;
			ItemInfo.InventoryIndex = InventoryIndex;
			ItemInfo.ItemStack = SellCountValue;
			Controller->ItemMoveToCart(ItemInfo);
		}
	}
}

void UFHSellCountBase::OnSellCountChanged()
{
	SellCount->SetText(FText::FromString(FString::FromInt(SellCountValue)));
}
