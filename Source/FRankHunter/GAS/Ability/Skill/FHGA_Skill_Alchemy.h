// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "FHGamePlaySkillAbility.h"
#include "Data/FHSkillTable.h"
#include "FHGA_Skill_Alchemy.generated.h"
/**
 * //==== GA_Skill_Alchemist ====//
 */
UCLASS()
class FRANKHUNTER_API UFHGA_Skill_Alchemy : public UFHGamePlaySkillAbility
{
	GENERATED_BODY()
	
public:
	UFHGA_Skill_Alchemy();

	UPROPERTY(EditDefaultsOnly, Category = "ItemSpawn")
	TSubclassOf<class AFHPickupItemActor> ChaosDistill;

	UPROPERTY(EditDefaultsOnly, Category = "ItemSpawn", meta = (ClampMin = "100.0", UIMin = "100.0"))
	float ItemSpawnDistance{ 300.0f };

protected:
	virtual void ExecuteActiveAbility() override;
};
