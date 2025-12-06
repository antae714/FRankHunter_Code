// Copyright F Rank Hunter. All Rights Reserved.


#include "Props/FHTray.h"
#include "Components/BoxComponent.h"

#include "Item/Actors/FHPickupItemActor.h"
#include "Item/FHInventoryComponent.h"
#include "Item/FHItemBase.h"

#include "Core/FHFRankHunterSettings.h"
#include "Core/FHPropManager.h"
#include "Core/ManagerActorRegistrySubsystem.h"
#include "Lobby/FH_GS_LobbyGameState.h"
#include "Kismet/GameplayStatics.h"

#include "Net/UnrealNetwork.h"

#include "BluePrintFunctions/FHBlueprintFunctionLibrary.h"
#include "Core/FHSoundManagerActor.h"
#include "GAS/FHGameplayTags.h"

// Sets default values
AFHTray::AFHTray()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void AFHTray::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFHTray, TotalItemValue);
}

// Called when the game starts or when spawned
void AFHTray::BeginPlay()
{
	Super::BeginPlay();
	
	BoxCollider = FindComponentByTag<UBoxComponent>(TEXT("Sell"));
	ItemValueCheckBox = FindComponentByTag<UBoxComponent>(TEXT("ValueCheck"));
}

// Called every frame
void AFHTray::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFHTray::OnItemValueCheckBoxOverlapBegin(AActor* DroppedItem)
{
	if (!DroppedItem->IsA(PickupItemActorClass))
	{
		return;
	}

	if (bIsWorking)
	{
		return;
	}

	TSoftObjectPtr<UDataTable> ItemDataTable = UFHFRankHunterSettings::StaticClass()->GetDefaultObject<UFHFRankHunterSettings>()->ItemDataTable;

	UFHInventoryComponent* Inventory = DroppedItem->GetComponentByClass<UFHInventoryComponent>();
	FSiItemDataElement& Item = Inventory->GetItemArray().GetItemAtIndex(0);
	UFHItemBase* ItemInstance = Cast<UFHItemBase>(Item.ItemInstance);

	FFHItemData* ItemData = ItemDataTable->FindRow<FFHItemData>(ItemInstance->ItemID, TEXT("AFHSellTray|ItemID"));
	TotalItemValue += ItemData->SellPrice;
	OnRep_TotalItemValue();
}

void AFHTray::OnItemValueCheckBoxOverlapEnd(AActor* _DroppedItem)
{
	if (bIsWorking)
	{
		return;
	}

	TSoftObjectPtr<UDataTable> ItemDataTable = UFHFRankHunterSettings::StaticClass()->GetDefaultObject<UFHFRankHunterSettings>()->ItemDataTable;
	TArray<AActor*> DroppedItems;
	ItemValueCheckBox->GetOverlappingActors(DroppedItems, PickupItemActorClass);

	int32 UpdatedTotalItemValue = 0;

	for (AActor* DroppedItem : DroppedItems)
	{
		UFHInventoryComponent* Inventory = DroppedItem->GetComponentByClass<UFHInventoryComponent>();
		FSiItemDataElement& Item = Inventory->GetItemArray().GetItemAtIndex(0);
		UFHItemBase* ItemInstance = Cast<UFHItemBase>(Item.ItemInstance);

		FFHItemData* ItemData = ItemDataTable->FindRow<FFHItemData>(ItemInstance->ItemID, TEXT("AFHSellTray|ItemID"));
		UpdatedTotalItemValue += ItemData->SellPrice;
	}

	TotalItemValue = UpdatedTotalItemValue;
	OnRep_TotalItemValue();
}

void AFHTray::SellItem()
{
	TSoftObjectPtr<UDataTable> ItemDataTable = UFHFRankHunterSettings::StaticClass()->GetDefaultObject<UFHFRankHunterSettings>()->ItemDataTable;
	int32 TotalSellPrice = 0;

	TArray<AActor*> DroppedItems;
	BoxCollider->GetOverlappingActors(DroppedItems, PickupItemActorClass);

	for (AActor* DroppedItem : DroppedItems)
	{
		UFHInventoryComponent* Inventory = DroppedItem->GetComponentByClass<UFHInventoryComponent>();
		FSiItemDataElement& Item = Inventory->GetItemArray().GetItemAtIndex(0);
		UFHItemBase* ItemInstance = Cast<UFHItemBase>(Item.ItemInstance);

		FFHItemData* ItemData = ItemDataTable->FindRow<FFHItemData>(ItemInstance->ItemID, TEXT("AFHSellTray|ItemID"));
		TotalSellPrice += ItemData->SellPrice;

		DroppedItem->Destroy();
	}

	AFH_GS_LobbyGameState* GameState = Cast<AFH_GS_LobbyGameState>(UGameplayStatics::GetGameState(GetWorld()));
	GameState->AddMoney(TotalSellPrice);


	// ======== Play Sound =========
	if (HasAuthority())
	{
		AFHSoundManagerActor* SoundManager = UFHBlueprintFunctionLibrary::GetSoundManager(GetWorld());
		if (SoundManager)
		{
			SoundManager->PlaySound2DMulticastByTag(GET_GAMEPLAY_TAG("Sound.Event.TransferMoney"));
		}
	}

}

void AFHTray::OnRep_TotalItemValue()
{
	OnTotalItemValueChanged();
}
