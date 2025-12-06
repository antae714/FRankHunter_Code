// Copyright F Rank Hunter. All Rights Reserved.


#include "BTElement/Decorators/BTDecorator_TargetIsCloaked.h"
#include "AbilitySystemBlueprintLibrary.h" 
#include "AbilitySystemComponent.h" 
#include "GAS/FHGameplayTags.h"

bool UBTDecorator_TargetIsCloaked::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AActor* TargetActorValue = Cast<AActor>(TargetActor.GetValue(OwnerComp));
	if (!TargetActorValue)
	{
		return true;
	}

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActorValue);
	if (!TargetASC)
	{
		return true;
	}

	const bool bIsStealthed = TargetASC->HasMatchingGameplayTag(GET_GAMEPLAY_TAG_PLAYER_STATE_ISCLOAKING);

	return bIsStealthed;
}
