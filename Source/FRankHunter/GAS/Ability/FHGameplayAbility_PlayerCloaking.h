// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Skill/FHGamePlaySkillAbility.h"
#include "FHGameplayAbility_PlayerCloaking.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHGameplayAbility_PlayerCloaking : public UFHGamePlaySkillAbility
{
	GENERATED_BODY()

public:
	UFHGameplayAbility_PlayerCloaking();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* OwnerInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TSubclassOf<class UGameplayEffect> CloakingEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TSubclassOf<class UGameplayEffect> CooldownEffect;
};
