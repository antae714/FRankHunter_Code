// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Ability/Skill/FHGamePlaySkillAbility.h"
#include "FHGA_Skill_Stopper.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHGA_Skill_Stopper : public UFHGamePlaySkillAbility
{
	GENERATED_BODY()
	
public:
	UFHGA_Skill_Stopper();

protected:
	virtual void ExecuteActiveAbility() override;

	UPROPERTY(EditDefaultsOnly, Category = "Stopper Skill")
	TSubclassOf<class UGameplayEffect> StunEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Stopper Skill")
	TSubclassOf<class UGameplayEffect> PlayerStunEffectClass;
};
