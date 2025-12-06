// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "FHGamePlaySkillAbility.h"
#include "Data/FHSkillTable.h"
#include "FHGA_Skill_Breaker.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHGA_Skill_Breaker : public UFHGamePlaySkillAbility
{
	GENERATED_BODY()
	
public:
	UFHGA_Skill_Breaker();

protected:
	virtual void ApplyPassiveEffect() override;

	UPROPERTY(EditDefaultsOnly, Category = "Breaker")
	TSubclassOf<class UGameplayEffect> PassiveStatEffectClass;
};
