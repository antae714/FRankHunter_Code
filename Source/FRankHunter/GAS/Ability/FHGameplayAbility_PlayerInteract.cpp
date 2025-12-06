// Copyright F Rank Hunter. All Rights Reserved.


#include "GAS/Ability/FHGameplayAbility_PlayerInteract.h"
#include "GAS/FHGameplayTags.h"

UFHGameplayAbility_PlayerInteract::UFHGameplayAbility_PlayerInteract()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityInputID = EFHPlayerAbilityInputID::Interact;
	SetAssetTags(FGameplayTagContainer{ GET_GAMEPLAY_TAG_PLAYER_ABILITY_INTERACT });
}

void UFHGameplayAbility_PlayerInteract::ActivateAbility(const FGameplayAbilitySpecHandle Handle, 
														const FGameplayAbilityActorInfo* OwnerInfo, 
														const FGameplayAbilityActivationInfo ActivationInfo, 
														const FGameplayEventData* TriggerEventData)
{
	//if (OwnerInfo->AvatarActor.IsValid())
	//{
	//	AActor* AvatarActor = OwnerInfo->AvatarActor.Get();
	//	if (AvatarActor)
	//	{
	//		FHitResult hitResult;
	//		FVector start = AvatarActor->GetActorLocation();
	//		FVector end = start + AvatarActor->GetActorForwardVector() * 200.0f;
	//		FCollisionQueryParams collisionParams;
	//		collisionParams.AddIgnoredActor(AvatarActor);
	//		collisionParams.bTraceComplex = true;
	//		if (GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECC_Visibility, collisionParams))
	//		{
	//			IInteractableInterface* interactable = Cast<IInteractableInterface>(hitResult.GetActor());
	//			if (interactable)
	//			{
	//				interactable->IInteractableInterface(AvatarActor);
	//			}
	//		}
	//	}
	//}




}
