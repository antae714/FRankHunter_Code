// Copyright F Rank Hunter.. All Rights Reserved.


#include "FHInventoryComponent.h"
#include "Kismet\GameplayStatics.h"
#include "FHItemBase.h"
#include "Item/Actors/FHThrowItemActor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Math/UnrealMathUtility.h"
#include "GameFramework/PlayerState.h"
#include "Core/FHPlayerController.h"
#include "Player/FHPlayerBase.h"
#include "UI/ItemQuickSlot/FHItemQuickSlot.h"
#include "BluePrintFunctions/FHBlueprintFunctionLibrary.h"
#include "GameFramework/GameStateBase.h"


// Sets default values for this component's properties
UFHInventoryComponent::UFHInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	// ...
}

void UFHInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ThisClass, CurrentItemIndex, COND_OwnerOnly);

}

void UFHInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		Server_SetCurrentItemIndex(-1);
	}
}

void UFHInventoryComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

}

void UFHInventoryComponent::OnInventoryChangedFunction(int32 index)
{
	if (GetOwner()->HasAuthority() && CurrentItemIndex == index)
	{
		if (GetCurrentItem() == nullptr)
		{
			InformPlayerEquippedItemDisappeard();
		}
		Server_SetCurrentItemIndex(index);
	}
	if (GetItemInstance<UFHItemBase>(index))
	{
		GetItemInstance<UFHItemBase>(index)->ChangeStackCount(GetItemStack(index));
	}
	Super::OnInventoryChangedFunction(index);
}

void UFHInventoryComponent::RefreshASCInit()
{
	for (size_t i = 0; i < GetItemNum(); i++)
	{
		if (GetItemInstance(i))
		{
			GetItemInstance(i)->SetOwnerComp(this);
			GetItemInstance<UFHItemBase>(i)->ChangeStackCount(GetItemStack(i));
		}
	}
}

void UFHInventoryComponent::ClearInventory()
{
	for (size_t i = 0; i < GetItemNum(); i++)
	{
		Server_RemoveItemAtIndex(i, GetItemStack(i));
	}
}

void UFHInventoryComponent::Server_SetCurrentItemIndex_Implementation(int32 NewItemIndex)
{
	UFHItemBase* CurrentItem = GetCurrentItem();
	if (CurrentItem)
	{
		CurrentItem->Server_SetEquipped(false);
	}
	CurrentItemIndex = NewItemIndex;

	OnRep_CurrentItemIndex();

	CurrentItem = GetCurrentItem();
	EItemHoldingType CurType = EItemHoldingType::NoHanded;
	if (CurrentItem)
	{
		CurType = CurrentItem->GetItemData().HoldingType;
		CurrentItem->Server_SetEquipped(true);
	}
	InformPlayerChangeItemHoldingType(CurrentItem);
}

void UFHInventoryComponent::SetFocusEmpty()
{
	OnChangedFocus.Broadcast(-1);
}

void UFHInventoryComponent::OnRep_CurrentItemIndex()
{
	OnChangedFocus.Broadcast(CurrentItemIndex);
}

UFHItemBase* UFHInventoryComponent::GetCurrentItem()
{
	return GetItemInstance<UFHItemBase>(GetCurrentItemIndex());
}

void UFHInventoryComponent::InformPlayerChangeItemHoldingType(UFHItemBase* CurrentItem)
{
	APlayerState* PS = GetOwner<APlayerState>();
	AFHPlayerController* PC = PS ? Cast<AFHPlayerController>(PS->GetPlayerController()) : nullptr;
	if (PC)
	{
		AFHPlayerBase* Player = Cast<AFHPlayerBase>(PC->GetPawn());
		if (Player)
		{
			Player->OnChangeHoldingItemType(CurrentItem);
		}
	}
}

void UFHInventoryComponent::InformPlayerEquippedItemDisappeard()
{
	APlayerState* PS = GetOwner<APlayerState>();
	AFHPlayerController* PC = PS ? Cast<AFHPlayerController>(PS->GetPlayerController()) : nullptr;
	if (PC)
	{
		AFHPlayerBase* Player = Cast<AFHPlayerBase>(PC->GetPawn());
		if (Player)
		{
			Player->InformPlayerEquippedItemDIsappeard();
		}
	}
}

void UFHInventoryComponent::SetTargetInventory(UFHInventoryComponent* TargetInventory)
{
	TargetInventoryWeak = TargetInventory;
}
