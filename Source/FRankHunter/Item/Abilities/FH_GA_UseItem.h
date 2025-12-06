// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GAS/FHGameplayAbility.h"
#include "FH_GA_UseItem.generated.h"

/**
 * 사용시 내구도처리, 소모 처리하는 어빌리티
 */
UCLASS()
class FRANKHUNTER_API UFH_GA_UseItem : public UFHGameplayAbility
{
	GENERATED_BODY()

public:
	UFH_GA_UseItem();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
									const FGameplayAbilityActorInfo* ActorInfo,
									const FGameplayTagContainer* SourceTags,
									const FGameplayTagContainer* TargetTags,
									OUT FGameplayTagContainer* OptionalRelevantTags) const override;

	virtual void PreActivate(const FGameplayAbilitySpecHandle Handle, 
							 const FGameplayAbilityActorInfo* ActorInfo, 
							 const FGameplayAbilityActivationInfo ActivationInfo, 
							 FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, 
							 const FGameplayEventData* TriggerEventData);

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
								 const FGameplayAbilityActorInfo* ActorInfo,
								 const FGameplayAbilityActivationInfo ActivationInfo,
								 const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;



	UFUNCTION(BlueprintCallable)
	void UseSucess();

	UPROPERTY(BlueprintReadWrite)
	uint32 bIsSucess : 1;
};

UCLASS()
class FRANKHUNTER_API UFH_GA_ItemConsumeCondition : public UFHGameplayAbility
{
	GENERATED_BODY()

public:
	UFH_GA_ItemConsumeCondition();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
								 const FGameplayAbilityActorInfo* ActorInfo,
								 const FGameplayAbilityActivationInfo ActivationInfo,
								 const FGameplayEventData* TriggerEventData) override;

};

