// Copyright F Rank Hunter. All Rights Reserved.


#include "Item/Actors/FHIstallItemActor.h"
#include "Item/FHInventoryComponent.h"
#include "Item/FHItemInstallable.h"
#include "BluePrintFunctions/FHBlueprintFunctionLibrary.h"
#include "Item/Data/FHItemClassDataAsset.h"
#include "AbilitySystemComponent.h"
#include "GAS/FHGameplayAbility.h"

AFHIstallItemActor::AFHIstallItemActor()
{
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

	bReplicates = true;
}

void AFHIstallItemActor::BeginPlay()
{
	Super::BeginPlay();

	SetActorRotation(FRotator::ZeroRotator, ETeleportType::TeleportPhysics);


	StaticMeshComponent->IgnoreActorWhenMoving(GetInstigator(), true);
	ASC->InitAbilityActorInfo(this, this);

	SetReplicates(true);

}

void AFHIstallItemActor::OnInventoryChangedFunction(USiInventoryComponent* Inventory, int32 Index)
{
	Super::OnInventoryChangedFunction(Inventory, Index);
	if (!HasAuthority())
	{
		return;
	}
	check(GetInstigator());

	UFHItemInstallable* item = Inventory->GetItemInstance<UFHItemInstallable>(Index);
	if(item)
	{
		UFHItemClassDataAsset* ItemDA = UFHBlueprintFunctionLibrary::GetItemClassDataAsset();
		UStaticMesh* ItemMesh = ItemDA->GetItemMesh(item->GetItemData().MeshID).LoadSynchronous();

		StaticMeshComponent->SetStaticMesh(ItemMesh);
		StaticMeshComponent->SetSimulatePhysics(false);
		StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		FVector InstigatorLocation = GetInstigator()->GetActorLocation();
		FVector InstigatorForward = GetInstigator()->GetControlRotation().Vector();
		SetActorLocation(InstigatorLocation, false, nullptr, ETeleportType::TeleportPhysics);


		StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		StaticMeshComponent->SetSimulatePhysics(true);

		SetActorLocation(InstigatorLocation + InstigatorForward * item->InstallLength, true);


		if (LatestInstallAbilitySpec.Ability)
		{
			return;
		}
		for (size_t i = 0; i < InventoryComponent->GetItemNum(); i++)
		{
			if (item && item->InstallAbility)
			{
				LatestInstallAbilitySpec = FGameplayAbilitySpec(item->InstallAbility);
				if (ASC)
				{
					ASC->AbilityEndedCallbacks.AddUObject(this, &ThisClass::OnInstallEnd);
					ASC->GiveAbilityAndActivateOnce(LatestInstallAbilitySpec);
				}

			}
		}
	}
}

void AFHIstallItemActor::OnInstallEnd(UGameplayAbility* EndAbility)
{
	if (ASC && LatestInstallAbilitySpec.Ability == EndAbility->GetClass()->GetDefaultObject())
	{
		Destroy(true);
	}
}



