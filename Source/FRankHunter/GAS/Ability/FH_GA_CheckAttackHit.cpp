// Copyright F Rank Hunter. All Rights Reserved.


#include "FH_GA_CheckAttackHit.h"
#include "FRankHunter.h"
#include "Player/FHPlayerBase.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GAS/FHGameplayTags.h"
#include "GAS/FHAttributeSet_PlayerStatus.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "Player/FHPlayerAnimInstance.h"
#include "DataAsset/FHPlayerAnimMontageDataAsset.h"
#include "GAS/Ability/Skill/FHGA_Skill_Breaker.h"
#include "Core/FHPlayerStateBase.h"

UFH_GA_CheckAttackHit::UFH_GA_CheckAttackHit()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UFH_GA_CheckAttackHit::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AFHPlayerBase* player = Cast<AFHPlayerBase>(ActorInfo->AvatarActor);
	if (!player)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	UAbilitySystemComponent* asc = player->GetAbilitySystemComponent();


	const AActor* target = TriggerEventData->Target.Get();
	if (!target)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	UAbilitySystemComponent* targetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(target);
	if (!targetASC)
	{
		// 여기서 Dynamic이면 힘 가하기?
		// test code
		/*FVector ForcedDirection = player->GetActorForwardVector();
		ForcedDirection.Normalize();
		ForcedDirection *= player->GetAttackPower() * PushPowerUnit;
		UStaticMeshComponent* meshComp = target->GetComponentByClass<UStaticMeshComponent>();
		if (meshComp && meshComp->Mobility == EComponentMobility::Movable && meshComp->IsSimulatingPhysics())
		{
			meshComp->AddImpulse(ForcedDirection);
		}*/

		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}



	float AttackPower{ 0.0f };
	float MiningPower{ 0.0f };
	bool isFounded{};

	if (TriggerEventData->TargetTags.HasTag(GET_GAMEPLAY_TAG("Player.Ability.Punch")))
	{
		AttackPower = 5.0f;
		MiningPower = 5.0f;

		if (asc->HasMatchingGameplayTag(GET_GAMEPLAY_TAG_SKILL_BREAKER))
		{
			MiningPower += player->GetMiningPower();
			
			TArray<FGameplayAbilitySpec*> BreakerAbilitySpecs;
			asc->GetActivatableGameplayAbilitySpecsByAllMatchingTags(FGameplayTagContainer{ GET_GAMEPLAY_TAG_SKILL_BREAKER }, BreakerAbilitySpecs);

			if (BreakerAbilitySpecs.Num() > 0)
			{
				UFHGA_Skill_Breaker* BreakerAbility = Cast<UFHGA_Skill_Breaker>(BreakerAbilitySpecs[0]->Ability);
				if (BreakerAbility)
				{
					const FFHSkillTable_Breaker* RowData = BreakerAbility->GetSkillData<FFHSkillTable_Breaker>(TEXT("AddAttackPower"));
					if (RowData)
					{
						AFHPlayerStateBase* PS = Cast<AFHPlayerStateBase>(player->GetPlayerState());
						const int32 CurrentLevel = PS ? PS->GetPlayerLevel() : 1;
						const int32 BonusMiningPower = RowData->GetValueByLevel(CurrentLevel);

						MiningPower += BonusMiningPower;

						UE_LOG(LogTemp, Log, TEXT("Breaker Punch. Mining Power is now: %f"), MiningPower);
					}
				}
			}
		}
	}
	else
	{
		MiningPower = asc->GetGameplayAttributeValue(UFHAttributeSet_PlayerStatus::GetMiningPowerAttribute(), isFounded);
		
		AttackPower = asc->GetGameplayAttributeValue(UFHAttributeSet_PlayerStatus::GetAttackPowerAttribute(), isFounded);

		if (asc->HasMatchingGameplayTag(GET_GAMEPLAY_TAG_SKILL_BREAKER))
		{
			TArray<FGameplayAbilitySpec*> BreakerAbilitySpecs;
			asc->GetActivatableGameplayAbilitySpecsByAllMatchingTags(FGameplayTagContainer{ GET_GAMEPLAY_TAG_SKILL_BREAKER }, BreakerAbilitySpecs);

			if (BreakerAbilitySpecs.Num() > 0)
			{
				UFHGA_Skill_Breaker* BreakerAbility = Cast<UFHGA_Skill_Breaker>(BreakerAbilitySpecs[0]->Ability);
				if (BreakerAbility)
				{
					const FFHSkillTable_Breaker* RowData = BreakerAbility->GetSkillData<FFHSkillTable_Breaker>(TEXT("AddAttackPower"));
					if (RowData)
					{
						AFHPlayerStateBase* PS = Cast<AFHPlayerStateBase>(player->GetPlayerState());
						const int32 CurrentLevel = PS ? PS->GetPlayerLevel() : 1;
						const int32 BonusMiningPower = RowData->GetValueByLevel(CurrentLevel);

						MiningPower += BonusMiningPower;

						UE_LOG(LogTemp, Log, TEXT("Breaker Attack. Mining Power is now: %f"), MiningPower);
					}
				}
			}
		}
	}


	FGameplayEventData eventData{ *TriggerEventData };
	eventData.Instigator = player;

	if (targetASC->HasMatchingGameplayTag(GET_GAMEPLAY_TAG_ACTORTYPE_MAGICSTONE))
	{
		eventData.EventMagnitude = MiningPower;
		targetASC->HandleGameplayEvent(GET_GAMEPLAY_TAG_GAMEPLAYEVENT_HIT, &eventData);
	}
	else if (targetASC->HasMatchingGameplayTag(GET_GAMEPLAY_TAG_ACTORTYPE_CREATURE))
	{
		eventData.EventMagnitude = AttackPower;
		targetASC->HandleGameplayEvent(GET_GAMEPLAY_TAG_GAMEPLAYEVENT_HIT, &eventData);
	}
	else if (targetASC->HasMatchingGameplayTag(GET_GAMEPLAY_TAG_ACTORTYPE_PLAYER))
	{
		eventData.EventMagnitude = AttackPower * DamageByPlayerMultiplier;
		targetASC->HandleGameplayEvent(GET_GAMEPLAY_TAG_GAMEPLAYEVENT_HIT, &eventData);
	}
	else
	{
		eventData.EventMagnitude = AttackPower;
		targetASC->HandleGameplayEvent(GET_GAMEPLAY_TAG_GAMEPLAYEVENT_HIT, &eventData);
	}

	if (TriggerEventData->TargetTags.HasTag(GET_GAMEPLAY_TAG("Player.Ability.Punch")))
	{

	}
	else
	{
		SendGameplayEvent(GET_GAMEPLAY_TAG_GAMEPLAYEVENT_CONSUMECONDITION, FGameplayEventData());
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
