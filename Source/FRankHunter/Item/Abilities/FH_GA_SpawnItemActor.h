// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GAS/FHGameplayAbility.h"
#include "FH_GA_SpawnItemActor.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFH_GA_SpawnItemActor : public UFHGameplayAbility
{
	GENERATED_BODY()

public:
	UFH_GA_SpawnItemActor();
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
									const FGameplayAbilityActorInfo* ActorInfo,
									const FGameplayTagContainer* SourceTags,
									const FGameplayTagContainer* TargetTags,
									OUT FGameplayTagContainer* OptionalRelevantTags) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
								 const FGameplayAbilityActorInfo* ActorInfo,
								 const FGameplayAbilityActivationInfo ActivationInfo,
								 const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AFHWorldItemActor> ThrowItemActorClass;
};
