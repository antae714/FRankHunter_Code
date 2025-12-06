// Copyright F Rank Hunter. All Rights Reserved.


#include "Item/Actors/FHBackpack.h"
#include "Item/FHInventoryComponent.h"
#include "UI/Inventory/InventoryBase.h"
#include "Player/FHPlayerBase.h"
#include "Component/FHInteractableComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/FHPlayerBase.h"
#include "Core/FHPlayerStateBase.h"

#include "BluePrintFunctions/FHBlueprintFunctionLibrary.h"
#include "Core/FHSoundManagerActor.h"
#include "FRankHunter.h"

// Sets default values
AFHBackpack::AFHBackpack()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	InventoryCount = 4;
	InteractableComp = CreateDefaultSubobject<UFHInteractableComponent>(TEXT("InteractableComp"));

	InventoryComponent = CreateDefaultSubobject<UFHInventoryComponent>(TEXT("InventoryComponent"));

	bReplicates = true;
}

void AFHBackpack::BeginPlay()
{
	InventoryComponent->SetMaxItemCountBeforePlay(InventoryCount);
	Super::BeginPlay();

	InteractableComp->OnInteractClient.AddDynamic(this, &ThisClass::OnInteract_Impl);
	InteractableComp->OnInteractHoldClient.AddDynamic(this, &ThisClass::OnInteractHold_Impl);

}

void AFHBackpack::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bIsEquipped);
	DOREPLIFETIME(ThisClass, BackpackName);
}

USiInventoryComponent* AFHBackpack::GetInventoryComponent() const
{
	return InventoryComponent;
}

UAbilitySystemComponent* AFHBackpack::GetAbilitySystemComponent() const
{
	UAbilitySystemComponent* ReturnASC = nullptr;
	IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(PickedUpActor);
	if (AbilitySystemInterface)
	{
		ReturnASC = AbilitySystemInterface->GetAbilitySystemComponent();
	}
	return ReturnASC;
}

void AFHBackpack::SerializeData(FArchive& Ar)
{
	Ar.UsingCustomVersion(FFRankHunterCustomVersion::GUID);
	
	Ar << InventoryCount;
	Ar << BackpackName;

	OnRep_BackpackName();
}

void AFHBackpack::OnRep_BackpackName()
{
	OnChangedBackpackName(BackpackName);
}

void AFHBackpack::OnInteract_Impl(AFHPlayerBase* Player, const UInputAction* InputAction)
{
	if (OpenInventoryUIInputAction == InputAction)
	{
		if (!InventoryUIInstance)
		{
			InventoryUIInstance = CreateWidget<UInventoryBase>(Player->GetController<APlayerController>(), InventoryUIClass);
			InventoryUIInstance->AddToViewport(1);

			InventoryUIInstance->ActivateWidget();
			InventoryUIInstance->SetInventoryComponent(InventoryComponent);
		}
		else
		{
			// Set TargetInventoryComponent to use Item Quick Move.
			if (InventoryUIInstance->IsActivated())
			{
				InventoryUIInstance->DeactivateWidget();

			}
			else
			{
				InventoryUIInstance->ActivateWidget();
				InventoryUIInstance->SetInventoryComponent(InventoryComponent);
			}
		}

		UFHInventoryComponent* TargetInventoryComponent = Player->GetPlayerState()->GetComponentByClass<UFHInventoryComponent>();
		if (InventoryUIInstance->IsActivated())
		{
			InventoryComponent->SetTargetInventory(TargetInventoryComponent);
			TargetInventoryComponent->SetTargetInventory(InventoryComponent);
		}
		else
		{
			InventoryComponent->SetTargetInventory(nullptr);
			TargetInventoryComponent->SetTargetInventory(nullptr);
		}
	}
}

void AFHBackpack::OnInteractHold_Impl(AFHPlayerBase* Player, const UInputAction* InputAction, float HoldDuration)
{
	if (PickUpInputAction == InputAction)
	{
		Player->PickUpBackpack(this);
	}

	//Player->FindAssetData(TEXT("Backpack1"));
	//Player->FindAssetData(TEXT("Backpack2"));

}