// Copyright F Rank Hunter. All Rights Reserved.


#include "Item/Abilities/FH_GA_SpawnWorldItem.h"
#include "GAS/FHGameplayTags.h"
#include "Item/FHInventoryComponent.h"
#include "Item/Actors/FHWorldItemActor.h"
#include "Item/FHItemThrowable.h"
#include "Item/FHItemInstallable.h"
#include "Interfaces/SiInventorySystemInterface.h"
#include "Item/FHItemDropPointComponent.h"
#include "BluePrintFunctions\FHBlueprintFunctionLibrary.h"
#include "Core/FHSoundManagerActor.h"


UFH_GA_SpawnWorldItem::UFH_GA_SpawnWorldItem()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	ThrowItemActorClass = AFHWorldItemActor::StaticClass();
}

bool UFH_GA_SpawnWorldItem::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}
	AActor* AvatarActor = ActorInfo->AvatarActor.Get();
	if (!AvatarActor)
	{
		return false;
	}

	UFHInventoryComponent* Inventory = UFHBlueprintFunctionLibrary::GetInventoryComponent(AvatarActor);
	if (!Inventory)
	{
		return false;
	}
	if (Inventory->GetItemStack(Inventory->GetCurrentItemIndex()) <= 0)
	{
		return false;
	}
	return true;
}


void UFH_GA_SpawnWorldItem::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
											const FGameplayAbilityActorInfo* ActorInfo,
											const FGameplayAbilityActivationInfo ActivationInfo,
											const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);


	AActor* AvatarActor = ActorInfo->AvatarActor.Get();
	if (!AvatarActor)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	FSpawnItemParam Param;
	Param.ItemActorClass = ThrowItemActorClass;
	Param.SpawnLocation = K2_GetSpawnLocation(AvatarActor);

	if(Param.SpawnLocation == FVector())
	{
		UObject* DropPointObject = AvatarActor->FindComponentByInterface(UDropPointInterface::StaticClass());
		if (DropPointObject)
		{
			Param.SpawnLocation = IDropPointInterface::Execute_PopNextPoint(DropPointObject);
		}
		else
		{
			Param.SpawnLocation = AvatarActor->GetActorLocation();
		}
	}

	UFHBlueprintFunctionLibrary::SpawnItemToActor(AvatarActor, Param);
	
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UFH_GA_SpawnWorldItem::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

FVector UFH_GA_SpawnWorldItem::K2_GetSpawnLocation_Implementation(AActor* AvatarActor) const
{
	return FVector();
}
