// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GAS/FHGameplayAbility.h"
#include "FHGameplayAbility_PlayerInteract.generated.h"

/**
 * Obsolete
 */
UCLASS()
class FRANKHUNTER_API UFHGameplayAbility_PlayerInteract : public UFHGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFHGameplayAbility_PlayerInteract();


	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, 
								 const FGameplayAbilityActorInfo* OwnerInfo, 
								 const FGameplayAbilityActivationInfo ActivationInfo, 
								 const FGameplayEventData* TriggerEventData) override;

};
