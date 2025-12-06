// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GAS/FHGameplayAbility.h"
#include "FH_GA_SpawnWorldItem.generated.h"

/**
 * 실행조건 : 
 */
UCLASS(Blueprintable)
class FRANKHUNTER_API UFH_GA_SpawnWorldItem : public UFHGameplayAbility
{
	GENERATED_BODY()

public:
	UFH_GA_SpawnWorldItem();
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, 
									const FGameplayAbilityActorInfo* ActorInfo, 
									const FGameplayTagContainer* SourceTags, 
									const FGameplayTagContainer* TargetTags, 
									OUT FGameplayTagContainer* OptionalRelevantTags) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, 
								 const FGameplayAbilityActorInfo* ActorInfo, 
								 const FGameplayAbilityActivationInfo ActivationInfo, 
								 const FGameplayEventData* TriggerEventData) override;

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, 
							   const FGameplayAbilityActorInfo* ActorInfo, 
							   const FGameplayAbilityActivationInfo ActivationInfo) override;

	UFUNCTION(BlueprintNativeEvent)
	FVector K2_GetSpawnLocation(AActor* AvatarActor) const;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AFHWorldItemActor> ThrowItemActorClass;
};
