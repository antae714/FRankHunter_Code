// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FRankHunter.h"
#include "FHGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFHGameplayAbility();

	// Ability trigger input setting
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FHAbility")
	EFHPlayerAbilityInputID AbilityInputID = EFHPlayerAbilityInputID::None;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FHAbility")
	uint8 bCancleAbilityOnInputRelease : 1{ false };

	// Activate granted abilities on initializing sequence
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "FHAbility")
	uint8 bActivateAbilityOnGranted : 1{ false };

	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

};
