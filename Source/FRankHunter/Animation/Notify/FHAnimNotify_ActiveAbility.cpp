// Copyright F Rank Hunter. All Rights Reserved.


#include "Animation/Notify/FHAnimNotify_ActiveAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

void UFHAnimNotify_ActiveAbility::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (!MeshComp || !MeshComp->GetWorld())
	{
		return;
	}
	AActor* OwnerActor = MeshComp->GetOwner();


	UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerActor);
	if (AbilitySystemComponent)
	{
		switch (AbilityActiveType)
		{
		case EAbilityActiveType::Tag:
		{
			AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer{ AbilityTag });
		}
		break;
		case EAbilityActiveType::Class:
		{
			AbilitySystemComponent->TryActivateAbilityByClass(AbilityClass);
		}
		break;
		default:
			break;
		}
	}
}
