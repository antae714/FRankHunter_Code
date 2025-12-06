// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GAS/FHGameplayAbility.h"
#include "FH_GA_PlayerPunchAttack.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFH_GA_PlayerPunchAttack : public UFHGameplayAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameplayCue")
    FGameplayTag MontageCueTag_Punch;

public:
	UFH_GA_PlayerPunchAttack();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
public:
	UFUNCTION()
	void OnMontageEnd();

protected:
	UPROPERTY()
	class UAbilityTask_PlayMontageAndWait* MontageTask;

	UPROPERTY()
	class AFHPlayerBase* Player{ nullptr };

	float AbilityDuration{ 0.5f };
	UFUNCTION()
	void OnTimerSet();
};
