// Copyright F Rank Hunter. All Rights Reserved.


#include "GAS/Ability/FHGameplayAbility_PlayerCloaking.h"
#include "GAS/FHGameplayTags.h"
#include "Player/FHPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"

UFHGameplayAbility_PlayerCloaking::UFHGameplayAbility_PlayerCloaking()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityInputID = EFHPlayerAbilityInputID::Cloaking;
	SetAssetTags(FGameplayTagContainer{ GET_GAMEPLAY_TAG_PLAYER_ABILITY_CLOAKING });

	ActivationBlockedTags.AddTag(GET_GAMEPLAY_TAG_PLAYER_ABILITY_CLOAKING);
	ActivationBlockedTags.AddTag(GET_GAMEPLAY_TAG_PLAYER_COOLDOWN_CLOAKING);
}

bool UFHGameplayAbility_PlayerCloaking::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
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

	const bool bIsCrouching = Player->GetCharacterMovement()->IsCrouching();
	const bool bIsVelocityZero = Player->GetVelocity().IsNearlyZero();

	if (bIsCrouching && bIsVelocityZero)
	{
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't Activate Cloaking."));
	}

	return false;
}

void UFHGameplayAbility_PlayerCloaking::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* OwnerInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, OwnerInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, OwnerInfo, ActivationInfo))
	{
		EndAbility(Handle, OwnerInfo, ActivationInfo, true, true);
		return;
	}

	AFHPlayerBase* Player = Cast<AFHPlayerBase>(OwnerInfo->AvatarActor);
	if (!Player)
	{
		EndAbility(Handle, OwnerInfo, ActivationInfo, true, true);
		return;
	}

	ApplyGameplayEffectToOwner(Handle, OwnerInfo, ActivationInfo, CloakingEffect.GetDefaultObject(), 1.0f);

	//ApplyGameplayEffectToOwner(Handle, OwnerInfo, ActivationInfo, CooldownEffect.GetDefaultObject(), 1.0f);
}

void UFHGameplayAbility_PlayerCloaking::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	// TODO : 은신이 강제 해제 되는 상황 대비 (소음 발생)
}

void UFHGameplayAbility_PlayerCloaking::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
