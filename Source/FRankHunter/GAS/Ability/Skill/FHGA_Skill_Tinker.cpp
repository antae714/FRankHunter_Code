// Copyright F Rank Hunter. All Rights Reserved.


#include "GAS/Ability/Skill/FHGA_Skill_Tinker.h"

UFHGA_Skill_Tinker::UFHGA_Skill_Tinker()
{
	SkillType = ESkillType::Toggle;

	ToggleStateTag = GET_GAMEPLAY_TAG_PLAYER_STATE_ISTINKERON;
}

void UFHGA_Skill_Tinker::OnToggleOn()
{
	UE_LOG(LogTemp, Log, TEXT("Tinker Toggled ON"));

	/* UFH_GA_ItemConsumeCondition::ActivateAbility */
}

void UFHGA_Skill_Tinker::OnToggleOff()
{
	UE_LOG(LogTemp, Log, TEXT("Tinker Toggled OFF"));

	/* UFH_GA_ItemConsumeCondition::ActivateAbility */
}
