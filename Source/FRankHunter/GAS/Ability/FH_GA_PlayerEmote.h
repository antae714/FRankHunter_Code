// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GAS/FHGameplayAbility.h"
#include "FH_GA_PlayerEmote.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFH_GA_PlayerEmote : public UFHGameplayAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	class AFHPlayerBase* Player{ nullptr };

	UPROPERTY()
	class UFHPlayerAnimInstance* AnimInstance{ nullptr };

	UPROPERTY()
	class UAbilityTask_PlayMontageAndWait* PlayMontageTask;

public:
	UPROPERTY(EditAnywhere)
	float CameraBoomLength = 200.0f;

public:
	UFH_GA_PlayerEmote();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void OnMontageEnded();
};
