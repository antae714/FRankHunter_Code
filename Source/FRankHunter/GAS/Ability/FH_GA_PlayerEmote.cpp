// Copyright F Rank Hunter. All Rights Reserved.


#include "GAS/Ability/FH_GA_PlayerEmote.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GAS/FHGameplayTags.h"
#include "Player/FHPlayerBase.h"
#include "Player/FHPlayerDescriptor.h"
#include "GAS/FHGameplayAbilityTargetDatas.h"
#include "DataAsset/FHPlayerAnimMontageDataAsset.h"
#include "Common/CommonItemEnum.h"
#include "GameFramework/SpringArmComponent.h"


UFH_GA_PlayerEmote::UFH_GA_PlayerEmote()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
	bRetriggerInstancedAbility = true;

	ActivationOwnedTags.AddTag(GET_GAMEPLAY_TAG_PLAYER_STATE_ISPLAYINGEMOTE);
}

void UFH_GA_PlayerEmote::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	Player = Cast<AFHPlayerBase>(ActorInfo->AvatarActor.Get());
	if (!Player)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	Player->CameraBoom->TargetArmLength = CameraBoomLength;
	Player->ShowThirdPerson(CameraBoomLength);

	EPlayerEmoteType EmoteType = static_cast<EPlayerEmoteType>(static_cast<int32>(TriggerEventData->EventMagnitude));
	UAnimMontage* EmoteMontage = Player->PlayerDescriptor->AnimMontages->PlayerEmote;
	FName StartSection;
	switch (EmoteType)
	{
	case EPlayerEmoteType::Clapping:
	{
		StartSection = TEXT("Clapping");
		break;
	}
	case EPlayerEmoteType::Dance_GangnamStyle:
	{
		StartSection = TEXT("GangnamStyle");
		break;
	}
	case EPlayerEmoteType::Pointing:
	{
		StartSection = TEXT("Pointing");
		break;
	}
	case EPlayerEmoteType::Salute:
	{
		StartSection = TEXT("Salute");
		break;
	}
	case EPlayerEmoteType::Dance_SodaPop:
	{
		StartSection = TEXT("SodaPop");
		break;
	}
	}

	if (HasAuthority(&ActivationInfo))
	{
		Player->Server_HideGrabMeshTemporarily();
	}

	PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayerEmote"), EmoteMontage, 1.0f, StartSection);
	PlayMontageTask->OnCompleted.AddDynamic(this, &UFH_GA_PlayerEmote::OnMontageEnded);
	PlayMontageTask->OnInterrupted.AddDynamic(this, &UFH_GA_PlayerEmote::OnMontageEnded);

	PlayMontageTask->ReadyForActivation();
}

void UFH_GA_PlayerEmote::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UFH_GA_PlayerEmote::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	if (!Player)
	{
		return;
	}

	Player->ShowFirstPerson(0.0f);

	if (HasAuthority(&ActivationInfo))
	{
		Player->Server_ShowGrabMeshBack();
	}
}

void UFH_GA_PlayerEmote::OnMontageEnded()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
