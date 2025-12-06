// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GAS/FHGameplayAbility.h"
#include "FHGameplayAbility_PlayerObserve.generated.h"

/** Observe Prev or Next */
UCLASS()
class FRANKHUNTER_API UFHGameplayAbility_PlayerObserve : public UFHGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFHGameplayAbility_PlayerObserve();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
	uint32 bIsObserveNext : 1 { true };

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* OwnerInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
