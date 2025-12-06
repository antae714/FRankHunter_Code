// Copyright F Rank Hunter. All Rights Reserved.


#include "FHBuyUIBase.h"
#include "FHItemListBase.h"
#include "Item/ItemTypes.h"
#include "Item/Data/FHItemClassDataAsset.h"
#include "BlueprintFunctions/FHBlueprintFunctionLibrary.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"

#include "Lobby/FH_GS_LobbyGameState.h"
#include "Core/FHPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "LobbyUIUtils.h"

#include "Props/FHKioskBase.h"
#include "Component/WidgetRPC/FHBuyRPCComponent.h"

void UFHBuyUIBase::NativeConstruct()
{
	Super::NativeConstruct();

	if (RPCComponent == nullptr)
	{
		APlayerController* LocalController = GetOwningPlayer();
		if (!LocalController)
		{
			return;
		}
		RPCComponent = LocalController->GetComponentByClass<UFHBuyRPCComponent>();
		//RPCComponent = NewObject<UFHBuyRPCComponent>(LocalController);
		//RPCComponent->RegisterComponent();
		//LocalController->AttachToComponent(LocalController->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
	}
	AFH_GS_LobbyGameState* GameState = Cast<AFH_GS_LobbyGameState>(UGameplayStatics::GetGameState(GetWorld()));
	if (GameState)
	{
		GameState->OnChangedPlayerMoney.AddDynamic(this, &ThisClass::OnPlayerMoneyChanged);
	}
}

void UFHBuyUIBase::InitShopList()
{
	if (bIsInitialized)
	{
		return;
	}

	int32 MaxItemCountInTab = 0;

	AFHKioskBase* Kiosk = Cast<AFHKioskBase>(UGameplayStatics::GetActorOfClass(GetWorld(), AFHKioskBase::StaticClass()));
	UFHItemClassDataAsset* DataAsset = UFHBlueprintFunctionLibrary::GetItemClassDataAsset();
	if (ensure(Kiosk) && ensure(DataAsset))
	{
		for (auto& ItemList : Kiosk->GetShopList().Items)
		{
			FFHItemData* ItemData = ItemTable->FindRow<FFHItemData>(ItemList.ItemID, TEXT("ItemID"));
			if (ItemData)
			{
				UFHItemListBase* ItemListUI = CreateItemListInstance();
				check(ItemListUI);

				if (ItemData->MaxBuyCount <= 0)
				{
					continue;
				}

				//ItemListUI->InitLayOut(EUIType::Buy);
				//ItemListUI->bIsBuyListUI = true;
				ItemListUI->ItemID = ItemData->ItemID;
				ItemListUI->ItemIcon->SetBrushFromSoftTexture(DataAsset->GetItemIcon(ItemData->IconID));
				ItemListUI->ItemName->SetText(DataAsset->GetItemName(ItemData->ItemNameID));
				//ItemListUI->ItemDescription->SetText(DataAsset->GetItemDescription(ItemData->DescriptionID));
				ItemListUI->Price->SetText(FText::FromString(FString::FromInt(ItemData->BuyPrice)));
				ItemListUI->Cost = ItemData->BuyPrice;
				ItemListUI->MaxBuyCount = ItemList.MaxBuyCount;
				ItemListUI->OnMaxBuyCountChanged();
				ItemListUI->CanBuyCount->SetText(FText::FromString(FString::FromInt(ItemList.MaxBuyCount)));
				ItemListUI->CachedItemData = *ItemData;
				FLinearColor ItemRarityColor = UFHBlueprintFunctionLibrary::GetItemRarityColor(ItemData->Rarity);
				ItemListUI->ItemName->SetColorAndOpacity(ItemRarityColor);

				EItemType ItemType = ItemList.ItemType;
				switch (ItemType)
				{
					case EItemType::Tool:
					{
						CachedEquipList.Add(ItemData->ItemID, ItemListUI);
						MaxItemCountInTab = FMath::Max(MaxItemCountInTab, CachedEquipList.Num());
						break;
					}
					case EItemType::ManaStone:
					{
						CachedUseList.Add(ItemData->ItemID, ItemListUI);
						MaxItemCountInTab = FMath::Max(MaxItemCountInTab, CachedUseList.Num());
						break;
					}
					case EItemType::Throwable:
					{
						CachedThrowList.Add(ItemData->ItemID, ItemListUI);
						MaxItemCountInTab = FMath::Max(MaxItemCountInTab, CachedThrowList.Num());
						break;
					}
					case EItemType::Consumable:
					{
						CachedUseList.Add(ItemData->ItemID, ItemListUI);
						MaxItemCountInTab = FMath::Max(MaxItemCountInTab, CachedUseList.Num());
						break;
					}
					case EItemType::Deployable:
					{
						CachedInstallList.Add(ItemData->ItemID, ItemListUI);
						MaxItemCountInTab = FMath::Max(MaxItemCountInTab, CachedInstallList.Num());
						break;
					}
					case EItemType::BackPack:
					{
						CachedEquipList.Add(ItemData->ItemID, ItemListUI);
						MaxItemCountInTab = FMath::Max(MaxItemCountInTab, CachedEquipList.Num());
						break;
					}
				}
			}
		}
	}
	//InitGrid(MaxItemCountInTab);
	bIsInitialized = true;
}

void UFHBuyUIBase::OnShopBuyCountChanged(const TArrayView<int32>& ChangedIndices, int32 FinalSize)
{
	AFHKioskBase* Kiosk = Cast<AFHKioskBase>(UGameplayStatics::GetActorOfClass(GetWorld(), AFHKioskBase::StaticClass()));
	if (ensure(Kiosk))
	{
		for (int32& ChangedIndex : ChangedIndices)
		{
			FItemList ItemList = Kiosk->GetShopList().Items[ChangedIndex];
			EItemType ItemType = ItemList.ItemType;

			TMap<FName, UFHItemListBase*>* CachedItemList = nullptr;
			switch (ItemType)
			{
			case EItemType::Tool:
			{
				CachedItemList = &CachedEquipList;
				break;
			}
			case EItemType::ManaStone:
			{
				CachedItemList = &CachedUseList;
				break;
			}
			case EItemType::Throwable:
			{
				CachedItemList = &CachedThrowList;
				break;
			}
			case EItemType::Consume:
			{
				CachedItemList = &CachedUseList;
				break;
			}
			case EItemType::Deployable:
			{
				CachedItemList = &CachedInstallList;
				break;
			}
			case EItemType::BackPack:
			{
				CachedItemList = &CachedEquipList;
				break;
			}

			}

			if (CachedItemList)
			{
				UFHItemListBase** ItemPtr = CachedItemList->Find(ItemList.ItemID);
				if (ItemPtr)
				{
					(*ItemPtr)->MaxBuyCount = ItemList.MaxBuyCount;
					(*ItemPtr)->OnMaxBuyCountChanged();
				}
			}
		}
	}
}

void UFHBuyUIBase::OnShopBuyCountChanged(const FName& ItemID, const int32 BuyCount)
{
	UFHItemListBase** ItemPtr = nullptr;

	ItemPtr = CachedEquipList.Find(ItemID);
	if (ItemPtr)
	{
		(*ItemPtr)->MaxBuyCount -= BuyCount;
		(*ItemPtr)->OnMaxBuyCountChanged();
		return;
	}

	ItemPtr = CachedUseList.Find(ItemID);
	if (ItemPtr)
	{
		(*ItemPtr)->MaxBuyCount -= BuyCount;
		(*ItemPtr)->OnMaxBuyCountChanged();
		return;
	}

	ItemPtr = CachedThrowList.Find(ItemID);
	if (ItemPtr)
	{
		(*ItemPtr)->MaxBuyCount -= BuyCount;
		(*ItemPtr)->OnMaxBuyCountChanged();
		return;
	}

	ItemPtr = CachedInstallList.Find(ItemID);
	if (ItemPtr)
	{
		(*ItemPtr)->MaxBuyCount -= BuyCount;
		(*ItemPtr)->OnMaxBuyCountChanged();
		return;
	}
}

UFHItemListBase* UFHBuyUIBase::CreateItemListInstance()
{
	UFHItemListBase* ItemListUI = CreateWidget<UFHItemListBase>(this, ItemListWidgetClass);
	if (ItemListUI)
	{
		ItemListUI->BuyUI = this;
		return ItemListUI;
	}
	return nullptr;
}

void UFHBuyUIBase::OnPlayerMoneyChanged(int32 Money)
{
	CurrentMoney->SetText(FText::FromString(FString::FromInt(Money)));
}

void UFHBuyUIBase::BackToHome()
{
	ResetCart();
	OnHomeButtonClickedDelegate.ExecuteIfBound();
}

void UFHBuyUIBase::OpenUI()
{
	if (!bIsInitialized)
	{
		InitShopList();
	}

	AFH_GS_LobbyGameState* GameState = Cast<AFH_GS_LobbyGameState>(UGameplayStatics::GetGameState(GetWorld()));
	if (GameState)
	{
		int32 PlayerMoney = GameState->GetMoney();
		CurrentMoney->SetText(FText::FromString(FString::FromInt(PlayerMoney)));
	}

	GetCachedItemList(EItemType::Tool);
}

void UFHBuyUIBase::ItemTypeTapClicked(int32 TapIndex)
{
	switch (TapIndex)
	{
	case 1:
	{
		GetCachedItemList(EItemType::Tool);
		break;
	}
	case 2:
	{
		GetCachedItemList(EItemType::Consume);
		break;
	}
	case 3:
	{
		GetCachedItemList(EItemType::Resource);
		break;
	}
	case 4:
	{
		GetCachedItemList(EItemType::ManaStone);
		break;
	}
	}
}

void UFHBuyUIBase::GetCachedItemList(EItemType ItemType)
{
	//ItemScrollList->ClearChildren();
	ResetGrid();
	switch (ItemType)
	{
	case EItemType::Tool:
	{
		for (auto& [ItemID, ItemList] : CachedEquipList)
		{
			AddItemInGrid(ItemList);
			//if (ItemList->MaxBuyCount == 0)
			//{
			//	continue;
			//}
			//ItemScrollList->AddChild(ItemList);
		}
		break;
	}
	case EItemType::ManaStone:
	{
		for (auto& [ItemID, ItemList] : CachedInstallList)
		{
			AddItemInGrid(ItemList);
			//if (ItemList->MaxBuyCount == 0)
			//{
			//	continue;
			//}
			//ItemScrollList->AddChild(ItemList);
		}
		break;
	}
	case EItemType::Resource:
	{
		for (auto& [ItemID, ItemList] : CachedThrowList)
		{
			AddItemInGrid(ItemList);
			//if (ItemList->MaxBuyCount == 0)
			//{
			//	continue;
			//}
			//ItemScrollList->AddChild(ItemList);
		}
		break;
	}
	case EItemType::Consume:
	{
		for (auto& [ItemID, ItemList] : CachedUseList)
		{
			AddItemInGrid(ItemList);
			//if (ItemList->MaxBuyCount == 0)
			//{
			//	continue;
			//}
			//ItemScrollList->AddChild(ItemList);
		}
		break;
	}
	}
}

void UFHBuyUIBase::AddToShoppingCart(FName ItemID, int32 Price)
{
	if (ShoppingCart.Find(ItemID))
	{
		ShoppingCart[ItemID]++;
	}
	else
	{
		ShoppingCart.Add(ItemID, 1);
	}
	TotalPrice += Price;
	OnTotalPriceChanged();
}

void UFHBuyUIBase::RemoveFromShoppingCart(FName ItemID, int32 Price)
{
	if (ShoppingCart.Find(ItemID))
	{
		ShoppingCart[ItemID]--;
		if (ShoppingCart[ItemID] == 0)
		{
			ShoppingCart.Remove(ItemID);
		}
	}

	TotalPrice -= Price;
	OnTotalPriceChanged();
}

void UFHBuyUIBase::ResetCart()
{
	if (!ShoppingCart.IsEmpty())
	{
		for (auto& [ItemID, BuyCount] : ShoppingCart)
		{
			UFHItemListBase** ItemPtr = nullptr;

			ItemPtr = CachedEquipList.Find(ItemID);
			if (ItemPtr)
			{
				(*ItemPtr)->ResetCount();
				continue;
			}

			ItemPtr = CachedUseList.Find(ItemID);
			if (ItemPtr)
			{
				(*ItemPtr)->ResetCount();
				continue;
			}

			ItemPtr = CachedThrowList.Find(ItemID);
			if (ItemPtr)
			{
				(*ItemPtr)->ResetCount();
				continue;
			}

			ItemPtr = CachedInstallList.Find(ItemID);
			if (ItemPtr)
			{
				(*ItemPtr)->ResetCount();
				continue;
			}
		}
		ShoppingCart.Empty();
	}

	TotalPrice = 0;
	OnTotalPriceChanged();
}

void UFHBuyUIBase::OnTotalPriceChanged()
{
	TotalPayment->SetText(FText::FromString(FString::FromInt(TotalPrice)));
}

void UFHBuyUIBase::OnPurchaseButtonClicked()
{
	if (ShoppingCart.Num() == 0)
	{
		return;
	}

	PurchaseItems.Empty();
	for (auto& [ItemID, ItemCount] : ShoppingCart)
	{
		FPurchaseData Data;
		Data.ItemID = ItemID;
		Data.ItemCount = ItemCount;
		PurchaseItems.Add(Data);
	}

	RPCComponent->PurchaseItem(PurchaseItems, TotalPrice);

	//AFHPlayerController* Controller = Cast<AFHPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	//if (Controller)
	//{
	//	Controller->PurchaseItem(PurchaseItems, TotalPrice);
	//}
}
