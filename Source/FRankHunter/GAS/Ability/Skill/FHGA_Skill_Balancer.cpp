// Copyright F Rank Hunter. All Rights Reserved.


#include "GAS/Ability/Skill/FHGA_Skill_Balancer.h"
#include "Core/FHPlayerStateBase.h"
#include "AbilitySystemComponent.h"

UFHGA_Skill_Balancer::UFHGA_Skill_Balancer()
{
	SkillType = ESkillType::Passive;
}

void UFHGA_Skill_Balancer::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (AFHPlayerStateBase* PS = Cast<AFHPlayerStateBase>(ActorInfo->OwnerActor.Get()))
	{
		PS->OnLevelUp.RemoveAll(this);
		PS->OnLevelUp.AddUObject(this, &ThisClass::OnPlayerLevelUp);

		LastAppliedLevel = PS->GetPlayerLevel();
	}
}

// server only
void UFHGA_Skill_Balancer::ApplyPassiveEffect()
{	
	GetAbilitySystemComponentFromActorInfo()->RemoveActiveEffectsWithTags(FGameplayTagContainer{ GET_GAMEPLAY_TAG_SKILL_BALANCER });

	UDataTable* LoadedTable = SkillTable.LoadSynchronous();
	if (!LoadedTable)
	{
		return;
	}

	AFHPlayerStateBase* PS = Cast<AFHPlayerStateBase>(GetActorInfo().OwnerActor.Get());
	const int32 CurrentLevel = PS ? PS->GetPlayerLevel() : 1;

	int32 VitalityToAdd = 0;
	int32 StrengthToAdd = 0;
	int32 EnduranceToAdd = 0;
	int32 AgilityToAdd = 0;
	int32 WillpowerToAdd = 0;
	int32 StatPointToAdd = 0;

	for (const TPair<FName, uint8*>& RowPair : LoadedTable->GetRowMap())
	{
		FName RowName = RowPair.Key;
		FFHSkillTable_Balancer* RowData = reinterpret_cast<FFHSkillTable_Balancer*>(RowPair.Value);
		if (!RowData) 
			continue;

		if (RowName == TEXT("AddVitality"))
		{
			VitalityToAdd = RowData->GetValueByLevel(CurrentLevel);
		}
		else if (RowName == TEXT("AddStrength"))
		{
			StrengthToAdd = RowData->GetValueByLevel(CurrentLevel);
		}
		else if (RowName == TEXT("AddEndurance"))
		{
			EnduranceToAdd = RowData->GetValueByLevel(CurrentLevel);
		}
		else if (RowName == TEXT("AddAgility"))
		{
			AgilityToAdd = RowData->GetValueByLevel(CurrentLevel);
		}
		else if (RowName == TEXT("AddWillpower"))
		{
			WillpowerToAdd = RowData->GetValueByLevel(CurrentLevel);
		}
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC && PassiveStatEffectClass)
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(PassiveStatEffectClass, GetAbilityLevel());

		if (SpecHandle.IsValid())
		{
			SpecHandle.Data->SetSetByCallerMagnitude(GET_GAMEPLAY_TAG_PLAYER_STATUS_VITALITY, VitalityToAdd);
			SpecHandle.Data->SetSetByCallerMagnitude(GET_GAMEPLAY_TAG_PLAYER_STATUS_STRENGTH, StrengthToAdd);
			SpecHandle.Data->SetSetByCallerMagnitude(GET_GAMEPLAY_TAG_PLAYER_STATUS_ENDURANCE, EnduranceToAdd);
			SpecHandle.Data->SetSetByCallerMagnitude(GET_GAMEPLAY_TAG_PLAYER_STATUS_AGILITY, AgilityToAdd);
			SpecHandle.Data->SetSetByCallerMagnitude(GET_GAMEPLAY_TAG_PLAYER_STATUS_WILLPOWER, WillpowerToAdd);

			ApplyGameplayEffectSpecToOwner(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), SpecHandle);

			UE_LOG(LogTemp, Log, TEXT("Balancer Skill On. Vit:%d, Str:%d, End:%d, Agi:%d, Wil:%d Up."),
				VitalityToAdd, StrengthToAdd, EnduranceToAdd, AgilityToAdd, WillpowerToAdd);		
		}
	}
}

void UFHGA_Skill_Balancer::OnPlayerLevelUp(uint32 NewLevel)
{
	bool bNeedsReapply = false;

	TArray<uint32> LevelTargets{ 10, 20, 30 };
	for (uint32 LevelTarget : LevelTargets)
	{
		if (LastAppliedLevel < LevelTarget && NewLevel >= LevelTarget)
		{
			if (LevelTarget == 10 || LevelTarget == 20 || LevelTarget == 30)
			{
				UDataTable* LoadedTable = SkillTable.LoadSynchronous();

				AFHPlayerStateBase* PS = Cast<AFHPlayerStateBase>(GetActorInfo().OwnerActor.Get());
				const int32 CurrentLevel = PS ? PS->GetPlayerLevel() : 1;

				const FFHSkillTable_Balancer* RowData = LoadedTable->FindRow<FFHSkillTable_Balancer>(TEXT("AddStatPoint"), TEXT(""));
				int32 StatPointToAdd = RowData->GetValueByLevel(CurrentLevel);
				if (StatPointToAdd > 0)
				{
					PS->AddStatPoints(StatPointToAdd);
				}

				UE_LOG(LogTemp, Log, TEXT("Balancer Skill : SP:%d Up."), StatPointToAdd);
			}
			// 대충 여기서 스탯 포인트 부여
			//bNeedsReapply = true;
		}
	}

	//if (bNeedsReapply)
	//{
	//	//ApplyPassiveEffect();
	//	//GetCurrentActorInfo()->AbilitySystemComponent->TryActivateAbility(GetCurrentAbilitySpecHandle());

	//	//UE_LOG(LogTemp, Log, TEXT("Player reached level %d. ReApply passive stats."), NewLevel);
	//}

	LastAppliedLevel = NewLevel;
}
