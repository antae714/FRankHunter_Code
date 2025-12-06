// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Ability/Skill/FHGamePlaySkillAbility.h"
#include "FHGA_Skill_Seeker.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHGA_Skill_Seeker : public UFHGamePlaySkillAbility
{
	GENERATED_BODY()

public:
	UFHGA_Skill_Seeker();

protected:
	virtual void ExecuteActiveAbility() override;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Seeker Skill")
	TSubclassOf<UGameplayEffect> DetectedEffectClass;
};
