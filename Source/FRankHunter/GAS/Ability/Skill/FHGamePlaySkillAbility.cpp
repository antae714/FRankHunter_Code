// Copyright F Rank Hunter. All Rights Reserved.


#include "GAS/Ability/Skill/FHGamePlaySkillAbility.h"
#include "AbilitySystemComponent.h"
#include "GAS/FHGameplayTags.h"
#include "Player/FHPlayerBase.h"
#include "DataAsset/FHPlayerAnimMontageDataAsset.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Core/FHPlayerStateBase.h"
#include "Core/FHPlayerController.h"
#include "UI/FHStatusHudBase.h"
#include "Camera/CameraShakeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Core/FHGateGameStateBase.h"

UFHGamePlaySkillAbility::UFHGamePlaySkillAbility()
{
	AbilityInputID = EFHPlayerAbilityInputID::UseSkill;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ServerOnlyTermination;
}

void UFHGamePlaySkillAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (SkillTable.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("Didn't find SkillTable."));
		return;
	}

	if (SkillType == ESkillType::Passive)
	{
		if (ActorInfo && ActorInfo->AbilitySystemComponent.IsValid())
		{
			ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle);
		}
	}
}

bool UFHGamePlaySkillAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
	{
		return false;
	}

	AGameStateBase* GameState = UGameplayStatics::GetGameState(GetWorld());
	AFHGateGameStateBase* GateGameState = Cast<AFHGateGameStateBase>(GameState);
	if (!GateGameState)
	{
		if (SkillType != ESkillType::Passive)
		{
			AFHPlayerStateBase* PS = Cast<AFHPlayerStateBase>(ActorInfo->OwnerActor.Get());
			if (PS)
			{
				PS->Client_OnBlockSkill_Implementation();
			}
			return false;
		}
	}

	const bool bCanActivate = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
	if (!bCanActivate)
	{
		AFHPlayerStateBase* PS = Cast<AFHPlayerStateBase>(ActorInfo->OwnerActor.Get());
		if (PS)
		{
			PS->Client_OnSkillCooldown_Implementation();
		}
		return false;
	}

	return true;
}

void UFHGamePlaySkillAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);


	// duration이 설정되지 않은(0.0f) CostGE를 적용하는 의미없는 부분인데 일단 놔두겠음
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}



	if (bHasAnimation)
	{
		AFHPlayerBase* Player = Cast<AFHPlayerBase>(GetAvatarActorFromActorInfo());
		if (Player)
		{
			UAnimMontage* MontageToPlay = Player->IsLocallyControlled() ?
				Player->GetAnimMontages()->FirstPersonUseSkillMontage : Player->GetAnimMontages()->ThirdPersonUseSkillMontage;

			if (MontageToPlay)
			{
				UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, MontageToPlay);
				if (MontageTask)
				{
					MontageTask->OnCompleted.AddDynamic(this, &UFHGamePlaySkillAbility::OnMontageCompleted);
					MontageTask->OnInterrupted.AddDynamic(this, &UFHGamePlaySkillAbility::OnMontageCancelled);
					MontageTask->OnCancelled.AddDynamic(this, &UFHGamePlaySkillAbility::OnMontageCancelled);
					MontageTask->ReadyForActivation();

					if (bWaitForMontageEnd)
					{
						return;
					}
				}
			}
		}
	}

	ExecuteSkill();
}

void UFHGamePlaySkillAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UFHGamePlaySkillAbility::ApplyCooldownWithLevelScaling()
{
	float CoolTimeValue = GetActiveSkillCooldown();
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGameplayEffectClass, 1);

	if (SpecHandle.IsValid())
	{
		SpecHandle.Data->SetDuration(CoolTimeValue, true);

		ApplyGameplayEffectSpecToOwner(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), SpecHandle);
	}
}

float UFHGamePlaySkillAbility::GetActiveSkillCooldown()
{
	UDataTable* LoadedTable = SkillTable.LoadSynchronous();
	if (!LoadedTable)
	{
		return 0.0f;
	}

	if (!CooldownGameplayEffectClass)
	{
		return 0.0f;
	}

	AFHPlayerStateBase* PS = Cast<AFHPlayerStateBase>(GetActorInfo().OwnerActor.Get());
	const int32 CurrentLevel = PS ? PS->GetPlayerLevel() : 1;

	const FFHSkillTable_ActiveBase* CoolTimeData = LoadedTable->FindRow<FFHSkillTable_ActiveBase>(TEXT("CoolTime"), TEXT(""));
	if (!CoolTimeData)
	{
		return 0.0f;
	}

	float Cooldown = CoolTimeData->GetValueByLevel(CurrentLevel);
	UE_LOG(LogTemp, Log, TEXT("CoolTime : %f second applied."), Cooldown);

	return Cooldown;
}

void UFHGamePlaySkillAbility::OnMontageCompleted()
{
	if (bWaitForMontageEnd)
	{
		ExecuteSkill();
	}
}

void UFHGamePlaySkillAbility::OnMontageCancelled()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
}

void UFHGamePlaySkillAbility::ExecuteSkill()
{
	const FGameplayAbilityActivationInfo Activationinfo = GetCurrentActivationInfo();

	switch (SkillType)
	{
	case ESkillType::Passive:
	{
		if (HasAuthority(&Activationinfo))
		{
			ApplyPassiveEffect();
		}

		break;
	}
	case ESkillType::Active:
	{
		ExecuteActiveAbility();

		if (HasAuthority(&Activationinfo))
		{
			ApplyCooldownWithLevelScaling();
		}

		if (IsLocallyControlled())
		{
			float CoolTimeValue = GetActiveSkillCooldown();
			AFHPlayerController* PC = Cast<AFHPlayerController>(GetWorld()->GetFirstPlayerController());
			if (PC)
			{
				PC->ClientStartCameraShake(CameraShakeClass);
				PC->StatusHudInstance->BP_OnStartSkillCooldown(this, CoolTimeValue);
			}
		}

		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
		break;
	}
	case ESkillType::Toggle:
	{
		UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
		if (!ASC)
		{
			EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
			return;
		}

		bool bIsToggledOn = ASC->HasMatchingGameplayTag(ToggleStateTag);
		if (bIsToggledOn)
		{
			ASC->RemoveLooseGameplayTag(ToggleStateTag);

			if (HasAuthority(&Activationinfo))
			{
				OnToggleOff();
			}
		}
		else
		{
			ASC->AddLooseGameplayTag(ToggleStateTag);

			if (HasAuthority(&Activationinfo))
			{
				OnToggleOn();
			}
		}

		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), Activationinfo, true, false);
		break;
	}
	default:
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), Activationinfo, true, true);
		break;
	}
}
