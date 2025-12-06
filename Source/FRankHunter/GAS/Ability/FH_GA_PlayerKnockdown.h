// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GAS/FHGameplayAbility.h"
#include "FH_GA_PlayerKnockdown.generated.h"

UCLASS()
class FRANKHUNTER_API UFH_GA_PlayerKnockdown : public UFHGameplayAbility
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UGameplayEffect> KnockdownEffect;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameplayCue")
    FGameplayTag MontageCueTag_WakeUpFront;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameplayCue")
    FGameplayTag MontageCueTag_WakeUpBack;


protected:
	FVector ThrowDirection;
	float ThrowPower;
	FName ImpulseTargetBone;
	bool isDead{ false };


public:
	UFH_GA_PlayerKnockdown();

	/*virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;*/

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;


protected:
	UFUNCTION()
	void ThrowItAway();
	UFUNCTION()
	void WakeUp();
	UFUNCTION()
	void AfterWakeUp();
};
