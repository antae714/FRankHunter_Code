// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Ability/Skill/FHGamePlaySkillAbility.h"
#include "FHGA_Skill_Breather.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHGA_Skill_Breather : public UFHGamePlaySkillAbility
{
	GENERATED_BODY()
	
public:
	UFHGA_Skill_Breather();

protected:
	virtual void ExecuteActiveAbility() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Breather Skill")
	TSubclassOf<class UGameplayEffect> CloakEffectClass;
};
