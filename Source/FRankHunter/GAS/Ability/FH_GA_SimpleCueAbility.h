// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GAS/FHGameplayAbility.h"
#include "FH_GA_SimpleCueAbility.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFH_GA_SimpleCueAbility : public UFHGameplayAbility
{
	GENERATED_BODY()

public:
	UFH_GA_SimpleCueAbility();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
								 const FGameplayAbilityActorInfo* ActorInfo,
								 const FGameplayAbilityActivationInfo ActivationInfo,
								 const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
							const FGameplayAbilityActorInfo* ActorInfo,
							const FGameplayAbilityActivationInfo ActivationInfo,
							bool bReplicateEndAbility,
							bool bWasCancelled);


	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle,
							   const FGameplayAbilityActorInfo* ActorInfo,
							   const FGameplayAbilityActivationInfo ActivationInfo) override;

	UPROPERTY(EditAnywhere)
	FGameplayTag CueTag;

};
