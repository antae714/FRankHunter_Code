// Copyright F Rank Hunter. All Rights Reserved.


#include "GAS/Ability/Skill/FHGA_Skill_Breather.h"
#include "Core/FHPlayerStateBase.h"

UFHGA_Skill_Breather::UFHGA_Skill_Breather()
{
	SkillType = ESkillType::Active;

	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	bHasAnimation = true;
	bWaitForMontageEnd = true;
}

void UFHGA_Skill_Breather::ExecuteActiveAbility()
{
	const FGameplayAbilityActivationInfo ActivationInfo = GetCurrentActivationInfo();
	if (HasAuthority(&ActivationInfo))
	{
		UDataTable* LoadedTable = SkillTable.LoadSynchronous();
		if (!LoadedTable || !CloakEffectClass)
		{
			return;
		}

		AFHPlayerStateBase* PS = Cast<AFHPlayerStateBase>(GetActorInfo().OwnerActor.Get());
		const int32 CurrentLevel = PS ? PS->GetPlayerLevel() : 1;

		const FFHSkillTable_Breather* DurationData = LoadedTable->FindRow<FFHSkillTable_Breather>(TEXT("Duration"), TEXT(""));
		if (!DurationData)
		{
			return;
		}

		const float Duration = DurationData->GetValueByLevel(CurrentLevel);

		UE_LOG(LogTemp, Log, TEXT("Breather Activated. Level=%d, Duration=%.1f"), CurrentLevel, Duration);

		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CloakEffectClass, CurrentLevel);
		if (SpecHandle.IsValid())
		{
			SpecHandle.Data.Get()->SetDuration(Duration, true);
			ApplyGameplayEffectSpecToOwner(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), SpecHandle);

			PS->Client_OnSkillFeedback(true);

			UE_LOG(LogTemp, Log, TEXT("Cloak for %.1f seconds."), Duration);
		}
	}
}
