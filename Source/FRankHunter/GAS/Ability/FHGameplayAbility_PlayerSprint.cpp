// Copyright F Rank Hunter. All Rights Reserved.


#include "GAS/Ability/FHGameplayAbility_PlayerSprint.h"
#include "GAS/FHGameplayTags.h"
#include "GameFramework/Character.h"
#include "Player/FHPlayerBase.h"
#include "Component/FHCharacterMovementComponent.h"

#include "Abilities/Tasks/AbilityTask_Repeat.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GAS/FHGameplayTags.h"


UFHGameplayAbility_PlayerSprint::UFHGameplayAbility_PlayerSprint()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityInputID = EFHPlayerAbilityInputID::Sprint;
	SetAssetTags(FGameplayTagContainer{ GET_GAMEPLAY_TAG_PLAYER_ABILITY_SPRINT });
	ActivationOwnedTags.AddTag(GET_GAMEPLAY_TAG_PLAYER_STATE_ISSPRINTING);
	ActivationBlockedTags.AddTag(GET_GAMEPLAY_TAG_PLAYER_STATE_ISEXHAUSTED);

	ConsumeStaminaInterval = 0.05f;
	ConsumeStaminaAmountPerSec = 10.0f;
	SquaredSpeedThreshold = 10.0f;
	SprintSpeedMultiplier = 1.4f;
}

bool UFHGameplayAbility_PlayerSprint::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	const AFHPlayerBase* MyPlayer = Cast<AFHPlayerBase>(ActorInfo->AvatarActor.Get());
	if (!MyPlayer || MyPlayer->GetCurrentStamina() <= 0.0f)
	{
		return false;
	}

	return true;
}

void UFHGameplayAbility_PlayerSprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}


		playerASC = ActorInfo->AbilitySystemComponent;
		if (!playerASC.IsValid())
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}
		player = Cast<AFHPlayerBase>(ActorInfo->AvatarActor);
		if (!player.IsValid())
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}
		playerMC = Cast<UFHCharacterMovementComponent>(player->GetMovementComponent());
		if (!playerMC.IsValid())
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}
		if (player->GetCurrentStamina() <= 0.0f)
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}

		ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
		if (!Character)
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}
		UFHCharacterMovementComponent* pmc = Cast<UFHCharacterMovementComponent>(Character->GetMovementComponent());
		if (!pmc)
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}

		pmc->StartSprinting();

		ConsumeStaminaPerTick = -ConsumeStaminaAmountPerSec * ConsumeStaminaInterval;

		if (!StaminaDrainEffect)
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}
		effectSpecHandle = playerASC->MakeOutgoingSpec(StaminaDrainEffect, 1, playerASC->MakeEffectContext());
		effectSpecHandle.Data->SetSetByCallerMagnitude(GET_GAMEPLAY_TAG_PLAYER_ABILITY_SPRINT, ConsumeStaminaPerTick);

		repeatConsumeStaminaEvent = UAbilityTask_Repeat::RepeatAction(this, ConsumeStaminaInterval, INT_MAX);
		repeatConsumeStaminaEvent->OnPerformAction.AddDynamic(this, &UFHGameplayAbility_PlayerSprint::ConsumeStamina);
		repeatConsumeStaminaEvent->ReadyForActivation();
	}
}

void UFHGameplayAbility_PlayerSprint::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (IsActive() == false) return;
	if (ActorInfo != NULL && ActorInfo->AvatarActor != NULL)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}
}

void UFHGameplayAbility_PlayerSprint::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	if (ScopeLockCount > 0)
	{
		WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &UFHGameplayAbility_PlayerSprint::CancelAbility, Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility));
		return;
	}

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	// 안전장치
	if (repeatConsumeStaminaEvent)
	{
		repeatConsumeStaminaEvent->EndTask();
	}

	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!Character) return;
	UFHCharacterMovementComponent* pmc = Cast<UFHCharacterMovementComponent>(Character->GetMovementComponent());
	if (!pmc) return;

	pmc->StopSprinting();
}

void UFHGameplayAbility_PlayerSprint::ConsumeStamina(int32 ActionNumber)
{
	if (!playerASC.IsValid() || !playerMC.IsValid()) return;

	if (playerMC->IsWalking() && playerMC->Velocity.SquaredLength() > SquaredSpeedThreshold)
		playerASC->ApplyGameplayEffectSpecToSelf(*effectSpecHandle.Data.Get());

	if (player->GetCurrentStamina() <= 0.0f)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}