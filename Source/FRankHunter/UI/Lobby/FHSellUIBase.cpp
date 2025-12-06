// Copyright F Rank Hunter. All Rights Reserved.


#include "UI/Lobby/FHSellUIBase.h"
#include "Kismet/GameplayStatics.h"
#include "Core/ManagerActorRegistrySubsystem.h"
#include "Core/FHPropManager.h"
#include "Core/FHFRankHunterSettings.h"
#include "Core/FHPlayerController.h"

#include "Props/FHCart.h"
#include "Item/FHItemBase.h"
#include "Item/FHInventoryComponent.h"

#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"

#include "FHSellSlot.h"
#include "FHItemRowSlot.h"

void UFHSellUIBase::NativeConstruct()
{
	Super::NativeConstruct();

	ItemDataTable = UFHFRankHunterSettings::StaticClass()->GetDefaultObject<UFHFRankHunterSettings>()->ItemDataTable;
}

void UFHSellUIBase::OpenUI()
{
	AFHPropManager* PropManager = GetWorld()->GetSubsystem<UManagerActorRegistrySubsystem>()->GetManagerActor<AFHPropManager>(TEXT("PropManager"));
	check(PropManager);

	AFHCart* Cart = PropManager->GetProp<AFHCart>(TEXT("Cart"));
	check(Cart);
	
	if (!Cart->GetComponentByClass<UFHInventoryComponent>()->OnInventoryChanged.IsAlreadyBound(this, &UFHSellUIBase::InventoryChanged))
	{
		Cart->GetComponentByClass<UFHInventoryComponent>()->OnInventoryChanged.AddDynamic(this, &UFHSellUIBase::InventoryChanged);
	}

	int32 InventoryMaxSize = Cart->GetComponentByClass<UFHInventoryComponent>()->GetItemNum();

	if (!bIsScrollListConstructed)
	{
		bIsScrollListConstructed = true;
		for (int32 Index = 0; ; Index += CountPerRow)
		{
			UFHItemRowSlot* SellSlotInstance = CreateWidget<UFHItemRowSlot>(this, SellSlotClass);
			check(SellSlotInstance);

			SellInventoryScroll->AddChild(SellSlotInstance);

			if (Index >= InventoryMaxSize)
			{
				break;
			}
		}
	}

	TArray<FSiItemDataElement>& ItemArray = Cart->GetComponentByClass<UFHInventoryComponent>()->GetItemArray().GetItemArray();
	UFHItemRowSlot* ItemSlotInstance = nullptr;

	for (int32 Index = 0; Index < InventoryMaxSize; Index++)
	{
		FSiItemDataElement& Item = ItemArray[Index];

		if (Item.ItemInstance == nullptr)
		{
			continue;
		}

		int32 Row = Index / CountPerRow;
		int32 Col = Index % CountPerRow;

		ItemSlotInstance = Cast<UFHItemRowSlot>(SellInventoryScroll->GetChildAt(Row));
		UFHSellSlot* SellSlot = Cast<UFHSellSlot>(ItemSlotInstance->GetItemSlot(Col));
		
		UFHItemBase* ItemBase = Cast<UFHItemBase>(Item.ItemInstance);
		SellSlot->ItemID = ItemBase->ItemID;
		SellSlot->ItemStackValue = Item.ItemStack;
		SellSlot->OnItemStackChanged();
		SellSlot->SetItemIcon();
		SellSlot->bHasItem = true;

		SellSlot->IncreaseSellPriceDelegate = FIncreaseSellPrice::CreateUObject(this, &UFHSellUIBase::SellItemAdded);
		SellSlot->DecreaseSellPriceDelegate = FDecreaseSellPrice::CreateUObject(this, &UFHSellUIBase::SellItemRemoved);
	}
}

void UFHSellUIBase::SellAll()
{
	UFHItemRowSlot* ItemSlotInstance = nullptr;
	for (int32 Row = 0; Row < SellInventoryScroll->GetChildrenCount(); Row++)
	{
		ItemSlotInstance = Cast<UFHItemRowSlot>(SellInventoryScroll->GetChildAt(Row));
		UFHSellSlot* SellSlot = nullptr;
		for (int32 Col = 0; Col < CountPerRow; Col++)
		{
			SellSlot = Cast<UFHSellSlot>(ItemSlotInstance->GetItemSlot(Col));
			if (!SellSlot->bHasItem)
			{
				continue;
			}
			int32 InventoryIndex = Row * CountPerRow + Col;

			FCachedSellItemInfo CachedSellItemInfo;
			CachedSellItemInfo.InventoryIndex = InventoryIndex;
			CachedSellItemInfo.ItemID = SellSlot->ItemID;
			CachedSellItemInfo.ItemStack = SellSlot->ItemStackValue;

			SellItemInfo.Add(CachedSellItemInfo);
		}
	}

	AFHPlayerController* Controller = Cast<AFHPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	Controller->SellItem(SellItemInfo, TotalSellPriceValue);
}

void UFHSellUIBase::Sell()
{
	UFHItemRowSlot* ItemSlotInstance = nullptr;
	for (int32 Row = 0; Row < SellInventoryScroll->GetChildrenCount(); Row++)
	{
		ItemSlotInstance = Cast<UFHItemRowSlot>(SellInventoryScroll->GetChildAt(Row));
		UFHSellSlot* SellSlot = nullptr;
		for (int32 Col = 0; Col < CountPerRow; Col++)
		{
			SellSlot = Cast<UFHSellSlot>(ItemSlotInstance->GetItemSlot(Col));
			if (!SellSlot->bIsSelected)
			{
				continue;
			}
			int32 InventoryIndex = Row * CountPerRow + Col;

			FCachedSellItemInfo CachedSellItemInfo;
			CachedSellItemInfo.InventoryIndex = InventoryIndex;
			CachedSellItemInfo.ItemID = SellSlot->ItemID;
			CachedSellItemInfo.ItemStack = SellSlot->SellCountValue;

			SellItemInfo.Add(CachedSellItemInfo);
		}
	}
	AFHPlayerController* Controller = Cast<AFHPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	Controller->SellItem(SellItemInfo, TotalSellPriceValue);
}

void UFHSellUIBase::BackToHome()
{
	OnHomeButtonClickedDelegate.ExecuteIfBound();
}

void UFHSellUIBase::SellItemAdded(FName ItemID)
{
	FFHItemData* ItemData = ItemDataTable->FindRow<FFHItemData>(ItemID, TEXT("UFHSellUIBase|ItemID"));
	check(ItemData);

	TotalSellPriceValue += ItemData->SellPrice;
	OnTotalSellPriceChanged();
}

void UFHSellUIBase::SellItemRemoved(FName ItemID)
{
	FFHItemData* ItemData = ItemDataTable->FindRow<FFHItemData>(ItemID, TEXT("UFHSellUIBase|ItemID"));
	check(ItemData);

	TotalSellPriceValue -= ItemData->SellPrice;
	OnTotalSellPriceChanged();
}

void UFHSellUIBase::InventoryChanged(class USiInventoryComponent* InventoryComponent, int32 index)
{
	const TArray<FSiItemDataElement>& ItemArray = InventoryComponent->GetItemArray().GetItemArray();
	const FSiItemDataElement& ItemElement = ItemArray[index];

	int32 Row = index / CountPerRow;
	int32 Col = index % CountPerRow;

	UFHItemRowSlot* ItemSlotInstance = Cast<UFHItemRowSlot>(SellInventoryScroll->GetChildAt(Row));
	UFHSellSlot* SellSlot = Cast<UFHSellSlot>(ItemSlotInstance->GetItemSlot(Col));

	if (ItemElement.ItemInstance == nullptr)
	{
		SellSlot->ResetSlot();
	}
	else
	{
		SellSlot->ItemStackValue = ItemElement.ItemStack;
		SellSlot->OnItemStackChanged();
		if (SellSlot->bIsFullSelected)
		{
			SellSlot->SellCountValue = ItemElement.ItemStack;
			SellSlot->OnSellCountChanged();
		}
	}
}

void UFHSellUIBase::OnTotalSellPriceChanged()
{
	TotalSellPrice->SetText(FText::FromString(FString::FromInt(TotalSellPriceValue)));
}
