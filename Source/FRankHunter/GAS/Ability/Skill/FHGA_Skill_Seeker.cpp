// Copyright F Rank Hunter. All Rights Reserved.


#include "GAS/Ability/Skill/FHGA_Skill_Seeker.h"
#include "Core/FHPlayerStateBase.h"
#include "Creature/FHCreatureBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Player/FHPlayerBase.h"

UFHGA_Skill_Seeker::UFHGA_Skill_Seeker()
{
	SkillType = ESkillType::Active;
	
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	bHasAnimation = true;
	bWaitForMontageEnd = true;
}

void UFHGA_Skill_Seeker::ExecuteActiveAbility()
{
	Super::ExecuteActiveAbility();

	const FGameplayAbilityActivationInfo ActivationInfo = GetCurrentActivationInfo();
	if (HasAuthority(&ActivationInfo))
	{
		UDataTable* LoadedTable = SkillTable.LoadSynchronous();
		if (!LoadedTable)
		{
			return;
		}

		AFHPlayerBase* Pawn = Cast<AFHPlayerBase>(GetAvatarActorFromActorInfo());
		if (!Pawn)
		{
			return;
		}

		AFHPlayerStateBase* PS = Cast<AFHPlayerStateBase>(GetActorInfo().OwnerActor.Get());
		const int32 CurrentLevel = PS ? PS->GetPlayerLevel() : 1;

		const FFHSkillTable_Seeker* RadiusData = LoadedTable->FindRow<FFHSkillTable_Seeker>(TEXT("Radius"), TEXT(""));
		const FFHSkillTable_Seeker* DurationData = LoadedTable->FindRow<FFHSkillTable_Seeker>(TEXT("Duration"), TEXT(""));
		if (!RadiusData || !DurationData)
		{
			return;
		}

		const float Radius = RadiusData->GetValueByLevel(CurrentLevel);
		const float Duration = DurationData->GetValueByLevel(CurrentLevel);

		if (!DetectedEffectClass)
		{
			return;
		}

		UE_LOG(LogTemp, Log, TEXT("Seeker Activated. Level=%d, Radius=%.1f, Duration=%.1f"), CurrentLevel, Radius, Duration);

		const AActor* AvatarActor = GetAvatarActorFromActorInfo();
		if (!AvatarActor)
		{
			return;
		}

		TArray<AActor*> OverlappedActors;
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

		UKismetSystemLibrary::SphereOverlapActors(GetWorld(), AvatarActor->GetActorLocation(), Radius, ObjectTypes, AFHCreatureBase::StaticClass(), TArray<AActor*>(), OverlappedActors);

		FGameplayAbilityTargetDataHandle TargetDataHandle;

		for (AActor* TargetActor : OverlappedActors)
		{
			FGameplayAbilityTargetData_ActorArray* NewTargetData = new FGameplayAbilityTargetData_ActorArray();
			NewTargetData->TargetActorArray.Add(TargetActor);

			TargetDataHandle.Add(NewTargetData);
		}

		if (TargetDataHandle.Num() > 0)
		{
			FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DetectedEffectClass, CurrentLevel);
			if (SpecHandle.IsValid())
			{
				SpecHandle.Data.Get()->SetDuration(Duration, true);

				ApplyGameplayEffectSpecToTarget(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), SpecHandle, TargetDataHandle);

				PS->Client_OnSkillFeedback(true);

				UE_LOG(LogTemp, Log, TEXT("Applied 'Detected' effect to %d monsters."), OverlappedActors.Num());
			}
		}
		else
		{
			PS->Client_OnSkillFeedback(false);

			UE_LOG(LogTemp, Log, TEXT("No Monsters Found in Range."));
		}

		Pawn->Multicast_PlaySeekerSkillEffect(Pawn->GetActorLocation(), Radius);
	}
}