// Copyright F Rank Hunter. All Rights Reserved.


#include "GAS/Ability/Skill/FHGA_Skill_Stopper.h"
#include "Core/FHPlayerStateBase.h"
#include "Player/FHPlayerBase.h"
#include "Creature/FHCreatureBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"

UFHGA_Skill_Stopper::UFHGA_Skill_Stopper()
{
	SkillType = ESkillType::Active;

	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	bHasAnimation = true;
	bWaitForMontageEnd = true;
}

void UFHGA_Skill_Stopper::ExecuteActiveAbility()
{
	const FGameplayAbilityActivationInfo ActivationInfo = GetCurrentActivationInfo();
	if (HasAuthority(&ActivationInfo))
	{
		UDataTable* LoadedTable = SkillTable.LoadSynchronous();
		if (!LoadedTable || !StunEffectClass || !PlayerStunEffectClass)
		{
			return;
		}

		AFHPlayerBase* Pawn = Cast<AFHPlayerBase>(GetAvatarActorFromActorInfo());
		if(!Pawn)
		{
			return;
		}

		AFHPlayerStateBase* PS = Cast<AFHPlayerStateBase>(GetActorInfo().OwnerActor.Get());
		const int32 CurrentLevel = PS ? PS->GetPlayerLevel() : 1;

		const FFHSkillTable_Stopper* RadiusData = LoadedTable->FindRow<FFHSkillTable_Stopper>(TEXT("Radius"), TEXT(""));
		const FFHSkillTable_Stopper* DurationData = LoadedTable->FindRow<FFHSkillTable_Stopper>(TEXT("Duration"), TEXT(""));
		const FFHSkillTable_Stopper* SelfStunData = LoadedTable->FindRow<FFHSkillTable_Stopper>(TEXT("bCausesSelfStun"), TEXT(""));
		if (!RadiusData || !DurationData || !SelfStunData)
		{
			return;
		}

		const float Radius = RadiusData->GetValueByLevel(CurrentLevel);
		const float Duration = DurationData->GetValueByLevel(CurrentLevel);
		const bool bCausesSelfStun = SelfStunData->GetValueByLevel(CurrentLevel) > 0;

		UE_LOG(LogTemp, Log, TEXT("Stopper Activated. Level=%d, Radius=%.1f, Duration=%.1f, SelfStun=%s"), CurrentLevel, Radius, Duration, bCausesSelfStun ? TEXT("True") : TEXT("False"));

		const AActor* AvatarActor = GetAvatarActorFromActorInfo();
		if (!AvatarActor)
		{
			return;
		}

		TArray<AActor*> OverlappedActors;
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
		UKismetSystemLibrary::SphereOverlapActors(GetWorld(), Pawn->GetActorLocation(), Radius, ObjectTypes, APawn::StaticClass(), TArray<AActor*>(), OverlappedActors);

		TArray<AActor*> CreatureTargets;
		TArray<AActor*> PlayerTargets;
		for (AActor* OverlappedActor : OverlappedActors)
		{
			if (OverlappedActor && OverlappedActor != Pawn)
			{
				if (OverlappedActor->IsA(AFHCreatureBase::StaticClass()))
				{
					CreatureTargets.Add(OverlappedActor);
				}
				else if (OverlappedActor->IsA(AFHPlayerBase::StaticClass()))
				{
					PlayerTargets.Add(OverlappedActor);
				}
			}
		}

		if (CreatureTargets.Num() > 0 && StunEffectClass)
		{
			FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(StunEffectClass, CurrentLevel);
			if (SpecHandle.IsValid())
			{
				SpecHandle.Data.Get()->SetDuration(Duration, true);

				FGameplayAbilityTargetDataHandle TargetDataHandle;
				for (AActor* TargetActor : CreatureTargets)
				{
					FGameplayAbilityTargetData_ActorArray* NewData = new FGameplayAbilityTargetData_ActorArray();
					NewData->TargetActorArray.Add(TargetActor);
					TargetDataHandle.Add(NewData);
				}

				ApplyGameplayEffectSpecToTarget(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), SpecHandle, TargetDataHandle);
				UE_LOG(LogTemp, Log, TEXT("Applied Stun to %d Monsters."), CreatureTargets.Num());
			}
		}

		if (PlayerTargets.Num() > 0 && PlayerStunEffectClass)
		{
			FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(PlayerStunEffectClass, CurrentLevel);
			if (SpecHandle.IsValid())
			{
				SpecHandle.Data.Get()->SetDuration(Duration, true);

				FGameplayAbilityTargetDataHandle TargetDataHandle;
				for (AActor* TargetActor : PlayerTargets)
				{
					FGameplayAbilityTargetData_ActorArray* NewData = new FGameplayAbilityTargetData_ActorArray();
					NewData->TargetActorArray.Add(TargetActor);
					TargetDataHandle.Add(NewData);
				}

				ApplyGameplayEffectSpecToTarget(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), SpecHandle, TargetDataHandle);
				UE_LOG(LogTemp, Log, TEXT("Applied Stun to %d Players."), PlayerTargets.Num());
			}
		}

		if ((CreatureTargets.Num() > 0 && StunEffectClass) || (PlayerTargets.Num() > 0 && PlayerStunEffectClass))
		{
			PS->Client_OnSkillFeedback(true);
		}
		else
		{
			PS->Client_OnSkillFeedback(false);
			UE_LOG(LogTemp, Log, TEXT("No other Monsters or Players Found in Range."));
		}

		if (bCausesSelfStun)
		{
			FGameplayEffectSpecHandle SelfSpecHandle = MakeOutgoingGameplayEffectSpec(PlayerStunEffectClass, CurrentLevel);
			if (SelfSpecHandle.IsValid())
			{
				SelfSpecHandle.Data.Get()->SetDuration(Duration, true);

				ApplyGameplayEffectSpecToOwner(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), SelfSpecHandle);

				UE_LOG(LogTemp, Log, TEXT("Player Stunned Self!"));
			}
		}

		Pawn->Multicast_PlayStopperSkillEffect(Pawn->GetActorLocation(), Radius);
	}
}