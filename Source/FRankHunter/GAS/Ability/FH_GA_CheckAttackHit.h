// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GAS/FHGameplayAbility.h"
#include "FH_GA_CheckAttackHit.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFH_GA_CheckAttackHit : public UFHGameplayAbility
{
	GENERATED_BODY()

	
public:
	UPROPERTY(EditAnywhere, Category=GAS)
	TSubclassOf<class UGameplayEffect> ApplyDamageToTargetEffect;

	// 이걸 여기다 두는 게 맞음????????
	UPROPERTY(EditAnywhere)
	float PushPowerUnit{ 2500.0f };
	UPROPERTY(EditAnywhere)
	float DamageByPlayerMultiplier{ 0.5f };

public:
	UFH_GA_CheckAttackHit();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
