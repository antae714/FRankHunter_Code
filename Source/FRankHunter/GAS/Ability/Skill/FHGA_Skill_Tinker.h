// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Ability/Skill/FHGamePlaySkillAbility.h"
#include "FHGA_Skill_Tinker.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHGA_Skill_Tinker : public UFHGamePlaySkillAbility
{
	GENERATED_BODY()
	
public:
	UFHGA_Skill_Tinker();

protected:
	virtual void OnToggleOn() override;
	virtual void OnToggleOff() override;

};
