// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "FHGamePlaySkillAbility.h"
#include "Data/FHSkillTable.h"
#include "FHGA_Skill_Balancer.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHGA_Skill_Balancer : public UFHGamePlaySkillAbility
{
	GENERATED_BODY()
	
public:
	UFHGA_Skill_Balancer();

protected:
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void ApplyPassiveEffect() override;

	UFUNCTION()
	void OnPlayerLevelUp(uint32 NewLevel);

	UPROPERTY(EditDefaultsOnly, Category = "Balancer")
	TSubclassOf<class UGameplayEffect> PassiveStatEffectClass;

private:
	uint32 LastAppliedLevel = 0;
};
