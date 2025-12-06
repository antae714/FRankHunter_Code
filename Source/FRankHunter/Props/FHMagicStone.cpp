// Copyright F Rank Hunter. All Rights Reserved.


#include "Props/FHMagicStone.h"
#include "FRankHunter.h"
#include "GAS/FHAbilitySystemComponent.h"
#include "GAS/FHGameplayTags.h"
#include "Item\FHInventoryComponent.h"
#include "Item\FHItemBase.h"
#include "Kismet\KismetSystemLibrary.h"
#include "Core/ManagerActorRegistrySubsystem.h"
#include "Item/Actors/ItemDropManagerActor.h"
#include "Item/FHItemDropPointComponent.h"
#include "Components/ChildActorComponent.h"
#include "Components/BoxComponent.h"
#include "GeometryCollection/GeometryCollectionComponent.h"

#include "BluePrintFunctions/FHBlueprintFunctionLibrary.h"
#include "Core/FHSoundManagerActor.h"


// Sets default values
AFHMagicStone::AFHMagicStone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	ItemDropPoint = CreateDefaultSubobject<UFHItemDropPoinManualComponent>(TEXT("ItemDropPoint"));
	InventoryComponent = CreateDefaultSubobject<UFHInventoryComponent>(TEXT("InventoryComponent"));
	InventoryComponent->SetMaxItemCountBeforePlay(1);

	HitBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBoxComponenet"));
	HitBoxComponent->AttachToComponent(GeometryCollectionComponent, FAttachmentTransformRules::KeepRelativeTransform);

	Health = 50.0f;
}

USiInventoryComponent* AFHMagicStone::GetInventoryComponent() const
{
	return InventoryComponent;
}

// Called when the game starts or when spawned
void AFHMagicStone::BeginPlay()
{
	InventoryComponent->SetMaxItemCountBeforePlay(HpPercent.Num());

	Super::BeginPlay();

	InitHealth = Health;

	if (HasAuthority())
	{
		for (size_t i = 0; i < HpPercent.Num(); i++)
		{
			UManagerActorRegistrySubsystem* ManagerActorRegistrySubsystem = GetWorld()->GetSubsystem<UManagerActorRegistrySubsystem>();
			check(ManagerActorRegistrySubsystem);

			AItemDropManagerActor* ItemDropManager = ManagerActorRegistrySubsystem->GetManagerActor<AItemDropManagerActor>(TEXT("ItemDropManagerActor"));
		if (ensureMsgf(ItemDropManager, TEXT("FHMagicStone is must have ItemDropManagerActor")))
			{
				TSubclassOf<UFHItemBase> ItemClass = ItemDropManager->GetSpawnManaStoneInfo();
				InventoryComponent->Server_AddItem(ItemClass, 1, i);
			}

		}
	}

	if (ItemDropPoint)
	{
		for (auto& item : ChildDestructionPoints)
		{
			if (!IsValid(item)) continue;
			ItemDropPoint->ManualPoints.Add(item->GetComponentLocation());
		}
	}
}

void AFHMagicStone::OnHealthChanged(float NewHealth)
{
	if (HasAuthority())
	{
		K2_OnHealthChanged(NewHealth);
		InventoryComponent->Server_SetCurrentItemIndex(CurrentIndex);
		AbilitySystem->TryActivateAbilitiesByTag(FGameplayTagContainer(GET_GAMEPLAY_TAG_PLAYER_ABILITY_DROPITEM));
	}


	// ======== Play Sound =========
	if (NewHealth <= 0.0f && bIsDestroyedFully == false)
	{
		bIsDestroyedHalf = true;
		bIsDestroyedFully = true;
		
		AFHSoundManagerActor* SoundManager = UFHBlueprintFunctionLibrary::GetSoundManager(GetWorld());
		if (SoundManager)
		{
			SoundManager->PlaySoundAtLocationLocallyByTag(GET_GAMEPLAY_TAG("Sound.Destroy.Manastone"), GetActorLocation(), 1.0f, true, 5.0f, this);
		}
	}
	else if (NewHealth <= InitHealth * 0.5f && bIsDestroyedHalf == false)
	{
		bIsDestroyedHalf = true;
		AFHSoundManagerActor* SoundManager = UFHBlueprintFunctionLibrary::GetSoundManager(GetWorld());
		if (SoundManager)
		{
			SoundManager->PlaySoundAtLocationLocallyByTag(GET_GAMEPLAY_TAG("Sound.Partial.Manastone"), GetActorLocation(), 1.0f, true, 4.0f, this);
		}
	}
}

void AFHMagicStone::OnRep_IsDestroy()
{
	Super::OnRep_IsDestroy();
	if (HitBoxComponent)
	{
		HitBoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

