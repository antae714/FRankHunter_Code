// Copyright F Rank Hunter. All Rights Reserved.


#include "UI/ItemQuickSlot/FHItemQuickSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "UI/ItemQuickSlot/FHItemQuickSlotElement.h"
#include "Item/FHItemBase.h"
#include "Item/FHInventoryComponent.h"
#include "BluePrintFunctions\FHBlueprintFunctionLibrary.h"
#include "GameFramework/PlayerState.h"



void UFHItemQuickSlot::NativeConstruct()
{
	Super::NativeConstruct();
	if (ItemsParent)
	{
		for (auto& item : ItemsParent->GetAllChildren())
		{
			UFHItemQuickSlotElement* ItemWidget = Cast<UFHItemQuickSlotElement>(Cast<UPanelWidget>(item)->GetChildAt(0));
			if (ItemWidget)
			{
				ItemSlots.Add(ItemWidget);
			}
		}

		APlayerState* PlayerState = GetOwningPlayerState();
		if (PlayerState)
		{
			UFHInventoryComponent* InventoryComponent = UFHBlueprintFunctionLibrary::GetInventoryComponent(PlayerState);
			if (InventoryComponent)
			{
				InventoryComponent->OnInventoryChanged.AddDynamic(this, &UFHItemQuickSlot::OnInventoryChanged);
				InventoryComponent->OnChangedFocus.AddDynamic(this, &UFHItemQuickSlot::ChageFocusItem);

				for (size_t i = 0; i < ItemSlots.Num(); i++)
				{
					OnInventoryChanged(InventoryComponent, i);
				}

				ChageFocusItem(InventoryComponent->GetCurrentItemIndex());
			}
		}
	}
}

void UFHItemQuickSlot::NativeDestruct()
{
	APlayerController* PlayerController = GetOwningPlayer();
	if (PlayerController)
	{
		UFHInventoryComponent* InventoryComponent = PlayerController->GetComponentByClass<UFHInventoryComponent>();
		if (InventoryComponent)
		{
			InventoryComponent->OnInventoryChanged.RemoveDynamic(this, &UFHItemQuickSlot::OnInventoryChanged);
		}
	}
}

void UFHItemQuickSlot::OnInventoryChanged(USiInventoryComponent* InventoryComponent, int32 index)
{
	if (InventoryComponent)
	{
		if (UFHItemBase* itemInstance = InventoryComponent->GetItemInstance<UFHItemBase>(index))
		{
			ItemSlots[index]->SetItemInfo(InventoryComponent, itemInstance, InventoryComponent->GetItemStack(index), index);
		}
		else
		{
			ItemSlots[index]->SetItemInfo(InventoryComponent, nullptr, 0, index);
		}
	}

}

void UFHItemQuickSlot::ChageFocusItem(int32 index)
{

	for (size_t i = 0; i < ItemsParent->GetSlots().Num(); i++)
	{
		UPanelSlot* PanelSlot = ItemsParent->GetSlots()[i];
		UHorizontalBoxSlot* HBox = Cast<UHorizontalBoxSlot>(PanelSlot);
		if (HBox)
		{
			FSlateChildSize Size = HBox->GetSize();
			Size.Value = i == index ? 1.4f : 1.0f;
			HBox->SetSize(Size);
		}
	}

	if (ItemSlots.IsValidIndex(index))
	{
		ItemSlots[CurrentFocusIndex]->SetFocus(false);
		ItemSlots[index]->SetFocus(true);

		CurrentFocusIndex = index;
	}
}
