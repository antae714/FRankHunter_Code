// Copyright F Rank Hunter. All Rights Reserved.


#include "GAS/Ability/Skill/FHGA_Skill_Breaker.h"
#include "Core/FHPlayerStateBase.h"
#include "AbilitySystemComponent.h"

UFHGA_Skill_Breaker::UFHGA_Skill_Breaker()
{
	SkillType = ESkillType::Passive;
}

void UFHGA_Skill_Breaker::ApplyPassiveEffect()
{
	GetAbilitySystemComponentFromActorInfo()->RemoveActiveEffectsWithTags(FGameplayTagContainer{ GET_GAMEPLAY_TAG_SKILL_BREAKER });

	UDataTable* LoadedTable = SkillTable.LoadSynchronous();
	if (!LoadedTable)
	{
		return;
	}

	AFHPlayerStateBase* PS = Cast<AFHPlayerStateBase>(GetActorInfo().OwnerActor.Get());
	const int32 CurrentLevel = PS ? PS->GetPlayerLevel() : 1;

	const FFHSkillTable_Breaker* RowData = LoadedTable->FindRow<FFHSkillTable_Breaker>(TEXT("AddAttackPower"), TEXT(""));
	if (!RowData)
	{
		return;
	}

	int32 AttackPowerToAdd = RowData->GetValueByLevel(CurrentLevel);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC && PassiveStatEffectClass)
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(PassiveStatEffectClass, GetAbilityLevel());
		if (SpecHandle.IsValid())
		{
			ApplyGameplayEffectSpecToOwner(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), SpecHandle);

			UE_LOG(LogTemp, Log, TEXT("Breaker Skill On. Mining Power On Punch: %d Up."), AttackPowerToAdd);
		}
	}
}