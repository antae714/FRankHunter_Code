// Copyright F Rank Hunter. All Rights Reserved.


#include "GAS/Ability/FH_GA_SimpleCueAbility.h"
#include "GAS/FHGameplayTags.h"
#include "AbilitySystemComponent.h"

UFH_GA_SimpleCueAbility::UFH_GA_SimpleCueAbility()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UFH_GA_SimpleCueAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	FGameplayAbilityTargetDataHandle TargetData = TriggerEventData->TargetData;
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (CueTag.IsValid())
	{
		FGameplayCueParameters CueParams;
		CueParams.SourceObject = GetCurrentSourceObject();

		GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(CueTag, CueParams);
	}
}

void UFH_GA_SimpleCueAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	if (CueTag.IsValid())
	{
		GetAbilitySystemComponentFromActorInfo()->RemoveGameplayCue(CueTag);
	}
}

void UFH_GA_SimpleCueAbility::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
