// Copyright F Rank Hunter. All Rights Reserved.


#include "GAS/Ability/FHGameplayAbility_PlayerCrouch.h"
#include "GAS/FHGameplayTags.h"
#include "GameFramework/Character.h"
#include "Player/FHPlayerBase.h"
#include "Component/FHCharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"

UFHGameplayAbility_PlayerCrouch::UFHGameplayAbility_PlayerCrouch()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityInputID = EFHPlayerAbilityInputID::Crouch;
	SetAssetTags(FGameplayTagContainer{ GET_GAMEPLAY_TAG_PLAYER_ABILITY_CROUCH });

	ActivationBlockedTags.AddTag(GET_GAMEPLAY_TAG_PLAYER_ABILITY_JUMP);
	ActivationOwnedTags.AddTag(GET_GAMEPLAY_TAG_PLAYER_STATE_ISCROUCHING);
	CancelAbilitiesWithTag.AddTag(GET_GAMEPLAY_TAG_PLAYER_ABILITY_SPRINT);
}

bool UFHGameplayAbility_PlayerCrouch::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags /*= nullptr*/, const FGameplayTagContainer* TargetTags /*= nullptr*/, OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr*/) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	AFHPlayerBase* Player = Cast<AFHPlayerBase>(ActorInfo->AvatarActor);
	if (!Player)
	{
		return false;
	}

	if (Player->GetCharacterMovement()->IsFalling())
	{
		return false;
	}

	return true;
}

void UFHGameplayAbility_PlayerCrouch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
		if (!Character) return;

		Character->Crouch();
	}
}

void UFHGameplayAbility_PlayerCrouch::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (IsActive() == false) return;
	if (ActorInfo != NULL && ActorInfo->AvatarActor != NULL)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}
}

void UFHGameplayAbility_PlayerCrouch::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	if (ScopeLockCount > 0)
	{
		WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &UFHGameplayAbility_PlayerCrouch::CancelAbility, Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility));
		return;
	}

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!Character) return;

	Character->UnCrouch();
}
