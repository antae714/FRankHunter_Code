// Copyright F Rank Hunter. All Rights Reserved.


#include "UI/Inventory/InventoryItemBase.h"
#include "GameFramework/PlayerState.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Item/FHItemBase.h"
#include "BluePrintFunctions\FHBlueprintFunctionLibrary.h"
#include "Item/Data/FHItemClassDataAsset.h"
#include "Item/FHInventoryComponent.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "UI/Common/ItemDrag.h"
#include "Components/PanelWidget.h"
#include "Components/SizeBox.h"
#include "Components/SizeBoxSlot.h"
#include "Components/ProgressBar.h"
#include "Blueprint/WidgetTree.h"
#include "UI/Lobby/FHItemListBase.h"
#include "UI/Lobby/FHSaleUIBase.h"
#include "UI/Lobby/LobbyUIUtils.h"


void UInventoryItemBase::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (!CachedInventory.Get())
	{
		SetItemInfo(nullptr, nullptr, 0, 0);
	}
}

void UInventoryItemBase::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	if(OutOperation == nullptr)
	{
		DragPayloadInstance = CreateWidget<UItemDragPayloadBase >(this, DragPayloadClass);
		DragPayloadInstance->SetPayloadWidget(this);

		UItemDrag* ItemDrag = Cast<UItemDrag>(UWidgetBlueprintLibrary::CreateDragDropOperation(UItemDrag::StaticClass()));
		ItemDrag->DefaultDragVisual = DragPayloadInstance;
		ItemDrag->Pivot = EDragPivot::MouseDown;
		ItemDrag->WidgetReference = this;
		OutOperation = ItemDrag;
	}

}

bool UInventoryItemBase::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bool Result = Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	if (!Result && !IsItemLock())
	{
		UItemDrag* ItemDrag = Cast<UItemDrag>(InOperation);
		if (ItemDrag)
		{
			SwapItem(ItemDrag->WidgetReference);

			;
			return Result;
		}

		UItemDragImage* ItemDragImage = Cast<UItemDragImage>(InOperation);
		if (ItemDragImage)
		{
			if (CachedItemInstance != nullptr)
			{
				if (CachedItemInstance->ItemID != ItemDragImage->ItemID)
				{
					return Result;
				}

				int32 MaxItemStack = CachedItemInstance->GetItemData().MaxStack;
				int32 CanInsertStack = MaxItemStack - CachedStack;

				if (CachedStack == MaxItemStack)
				{
					return Result;
				}

				ItemDragImage->OnItemDroppedDelegatePtr->ExecuteIfBound(
					ItemDragImage->ItemID,
					FMath::Min(ItemDragImage->ItemStack, CanInsertStack),
					CachedItemIndex,
					EItemMoveType::ShopToInventory
				);
			}
			else
			{
				UFHItemClassDataAsset* DataAsset = UFHBlueprintFunctionLibrary::GetItemClassDataAsset();
				if (DataAsset)
				{
					UFHItemBase* ItemCDO = DataAsset->GetItemClass(ItemDragImage->ItemID)->GetDefaultObject<UFHItemBase>();
					if (ItemCDO)
					{
						int32 MaxStack = ItemCDO->ItemMaxStack;

						ItemDragImage->OnItemDroppedDelegatePtr->ExecuteIfBound(
							ItemDragImage->ItemID,
							FMath::Min(ItemDragImage->ItemStack, MaxStack),
							CachedItemIndex,
							EItemMoveType::ShopToInventory
						);
					}
				}
			}
			return Result;
		}

	}
	return Result;
}

FReply UInventoryItemBase::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Result = FReply::Unhandled();

	const FModifierKeysState Mods = FSlateApplication::Get().GetModifierKeys();
	if (Mods.IsLeftControlDown())
	{
		if (CachedInventory->GetTargetInventory().IsValid() && CachedItemInstance.IsValid())
		{
			if (GetInventoryComponent()->GetOwner()->IsA<APlayerState>())
			{
				GetInventoryComponent()->Server_GiveItemToInventory2(GetInventoryComponent(), CachedItemIndex, 1, CachedInventory->GetTargetInventory().Get());
			}
			else
			{
				GetInventoryComponent()->GetTargetInventory()->Server_GiveItemToInventory2(GetInventoryComponent(), CachedItemIndex, 1, CachedInventory->GetTargetInventory().Get());
			}

			Result = FReply::Handled();
			return Result;
		}
	}

	Result = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	if(!Result.IsEventHandled())
	{
		if (CachedItemInstance.IsValid())
		{
			Result = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
		}
	}

	return Result;
}

void UInventoryItemBase::SetItemInfo(USiInventoryComponent* Inventory, UFHItemBase* itemInstance, int32 stack, int32 itemIndex)
{
	CachedInventory = Cast<UFHInventoryComponent>(Inventory);
	CachedItemInstance = itemInstance;
	CachedStack = stack;
	CachedItemIndex = itemIndex;

	if (itemInstance)
	{
		if (itemInstance->ItemMaxStack > 1 && itemInstance->ItemMaxStack <= stack)
		{
			ItemStack->SetColorAndOpacity(FSlateColor(FLinearColor(1.f, 0.f, 0.f, 1.f)));
		}
		else
		{
			ItemStack->SetColorAndOpacity(FSlateColor(FLinearColor(1.f, 1.f, 1.f, 1.f)));
		}

		ItemStack->SetText(FText::AsNumber(stack));
		UFHItemClassDataAsset* ItemDA = UFHBlueprintFunctionLibrary::GetItemClassDataAsset();
		check(ItemDA);

		TSoftObjectPtr<UTexture2D> SoftTexture = ItemDA->GetItemIcon(itemInstance->GetItemData().IconID);
		if (!SoftTexture.ToSoftObjectPath().IsValid())
		{
			SoftTexture = ItemNoneImage;
		}
		ItemImage->SetBrushFromSoftTexture(SoftTexture);
		ItemImage->SetVisibility(ESlateVisibility::Visible);

	}
	else
	{
		ItemStack->SetText(FText::GetEmpty());
		ItemImage->SetBrushFromTexture(nullptr);
		ItemImage->SetVisibility(ESlateVisibility::Hidden);
	}

	if (CachedInventory.Get())
	{
		if (LockImage && CachedInventory->IsItemLock(CachedItemIndex))
		{
			LockImage->SetVisibility(ESlateVisibility::Visible);
		}
		else if (LockImage)
		{
			LockImage->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}
UFHInventoryComponent* UInventoryItemBase::GetInventoryComponent() const
{
	return CachedInventory.Get();
}

UFHItemBase* UInventoryItemBase::GetItemInstance() const
{
	return CachedItemInstance.Get();
}

bool UInventoryItemBase::IsItemLock() const
{
	return GetInventoryComponent() ? GetInventoryComponent()->IsItemLock(CachedItemIndex) : false;
}

int32 UInventoryItemBase::GetInventoryIndex() const
{
	if (GetInventoryComponent() && CachedItemInstance.IsValid())
	{
		return GetInventoryComponent()->GetItemIndex(CachedItemInstance.Get());
	}
	else
	{
		return INDEX_NONE;
	}
}

void UInventoryItemBase::SwapItem(UInventoryItemBase* otherItemUIBase)
{
	if (!otherItemUIBase)
	{
		return;
	}

	if (this == otherItemUIBase)
	{
		return;
	}

	UFHInventoryComponent* OwnerShipInventoryComponent = GetInventoryComponent();
	UFHInventoryComponent* ThisInventoryComponent = GetInventoryComponent();
	UFHInventoryComponent* OtherInventoryComponent = otherItemUIBase->GetInventoryComponent();
	int32 ThisItemIndex = CachedItemIndex;
	int32 OtherItemIndex = otherItemUIBase->CachedItemIndex;

	bool bIsInventoryValid = ThisInventoryComponent && OtherInventoryComponent;
	if (bIsInventoryValid)
	{
		if (!OwnerShipInventoryComponent->GetOwner()->HasNetOwner())
		{
			OwnerShipInventoryComponent = OtherInventoryComponent;
		}
		if (!OwnerShipInventoryComponent->GetOwner()->HasNetOwner())
		{
			OwnerShipInventoryComponent = GetInventoryComponent()->GetTargetInventory().Get();
		}
		if (OwnerShipInventoryComponent)
		{
			OwnerShipInventoryComponent->Server_SwapItemWithInventory2(ThisInventoryComponent,
																  ThisItemIndex,
																  OtherInventoryComponent,
																  OtherItemIndex);
		}
	}
}

void UItemDragPayloadBase::SetPayloadWidget(UInventoryItemBase* Owner)
{
	OwnerWidget = Owner;
	OnSetPayloadWidget(Owner);
}

void UItemDragPayloadBase::NativeConstruct()
{
	Super::NativeConstruct();
}

void UItemDragImagePayloadBase::SetItemIcon(TSoftObjectPtr<UTexture2D> SoftTexture)
{
	ItemIcon->SetBrushFromSoftTexture(SoftTexture);
}

void UItemDragImagePayloadBase::NativeConstruct()
{
	Super::NativeConstruct();

	if (ensure(IconWidgetSlot))
	{
		IconWidgetSlot->SetWidthOverride(OwnerWidget->ItemIcon->GetCachedGeometry().GetLocalSize().X);
		IconWidgetSlot->SetHeightOverride(OwnerWidget->ItemIcon->GetCachedGeometry().GetLocalSize().Y);
		//UPanelSlot* BaseSlot = IconWidgetSlot->AddChild(ItemIcon);
	}
}
