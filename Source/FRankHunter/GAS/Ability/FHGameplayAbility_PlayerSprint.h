// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GAS/FHGameplayAbility.h"
#include "FHGameplayAbility_PlayerSprint.generated.h"

/**
 * DEPRECATED
 */
UCLASS()
class FRANKHUNTER_API UFHGameplayAbility_PlayerSprint : public UFHGameplayAbility
{
	GENERATED_BODY()

	UPROPERTY()
	class UAbilityTask_Repeat* repeatConsumeStaminaEvent;

	UPROPERTY()
	TWeakObjectPtr<class AFHPlayerBase> player;

	UPROPERTY()
	TWeakObjectPtr<class UAbilitySystemComponent> playerASC;

	UPROPERTY()
	TWeakObjectPtr<class UFHCharacterMovementComponent> playerMC;
	float ConsumeStaminaPerTick{ 0.0f };

	FGameplayEffectSpecHandle effectSpecHandle;

public:
	UPROPERTY(EditDefaultsOnly, Category = GAS)
	TSubclassOf<class UGameplayEffect> StaminaDrainEffect;

	UPROPERTY(EditDefaultsOnly, Category = GAS)
	float ConsumeStaminaAmountPerSec;

	UPROPERTY(EditDefaultsOnly, Category=GAS)
	float ConsumeStaminaInterval;

	UPROPERTY(EditDefaultsOnly, Category=GAS)
	float SquaredSpeedThreshold;

	UPROPERTY(EditDefaultsOnly, Category=GAS)
	float SprintSpeedMultiplier;


	UFHGameplayAbility_PlayerSprint();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* OwnerInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

protected:
	UFUNCTION()
	void ConsumeStamina(int32 ActionNumber);
};
