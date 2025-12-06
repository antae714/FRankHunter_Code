// Copyright F Rank Hunter. All Rights Reserved.


#include "Item/Abilities/FH_GA_SpawnItemActor.h"

UFH_GA_SpawnItemActor::UFH_GA_SpawnItemActor()
{
}

bool UFH_GA_SpawnItemActor::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	return false;
}

void UFH_GA_SpawnItemActor::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
}
