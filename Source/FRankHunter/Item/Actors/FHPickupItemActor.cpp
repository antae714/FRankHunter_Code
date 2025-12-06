// Copyright F Rank Hunter. All Rights Reserved.


#include "Item/Actors/FHPickupItemActor.h"
#include "Item/FHInventoryComponent.h"
#include "Player/FHPlayerBase.h"
#include "BluePrintFunctions/FHBlueprintFunctionLibrary.h"
#include "Component/FHInteractableComponent.h"
#include "Item/Actors/FHBackpack.h"



// Sets default values
AFHPickupItemActor::AFHPickupItemActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	InteractableComp = CreateDefaultSubobject<UFHInteractableComponent>(TEXT("InteractableComp"));
	bReplicates = true;
}

// Called when the game starts or when spawned
void AFHPickupItemActor::BeginPlay()
{
	Super::BeginPlay();
	
}


void AFHPickupItemActor::Interact_Impl(AFHPlayerBase* Player)
{
	UFHInventoryComponent* ToInventory = UFHBlueprintFunctionLibrary::GetInventoryComponent(Player);
	if (ToInventory)
	{
		for (size_t i = 0; i < InventoryComponent->GetItemNum(); i++)
		{
			int ToIndex = ToInventory->GetItemIndexIfAddable(InventoryComponent->GetItemClass(i), InventoryComponent->GetItemStack(i));
			if(ToIndex == INDEX_NONE)
			{
				if (Player->GetCurrentBackpackActor())
				{
					ToInventory = UFHBlueprintFunctionLibrary::GetInventoryComponent(Player->GetCurrentBackpackActor());
				}

			}
			int32 ItemStack = InventoryComponent->GetItemStack(i);
			InventoryComponent->Server_GiveItemToInventory(i, ItemStack, ToInventory);
		}
		int32 AllItemStack = 0;
		for (size_t i = 0; i < InventoryComponent->GetItemNum(); i++)
		{
			AllItemStack += InventoryComponent->GetItemStack(i);
		}
		if (AllItemStack <= 0)
		{
			Destroy();
		}
	}
}

void AFHPickupItemActor::Server_Interact_Implementation(UFHInventoryComponent* ToInventory)
{
	
}