// Copyright F Rank Hunter. All Rights Reserved.


#include "GAS/Ability/FH_GA_PlayerSwitchingTools.h"
#include "FRankHunter.h"
#include "GAS/FHGameplayTags.h"
#include "GAS/FHGameplayAbilityTargetDatas.h"
#include "Player/FHPlayerBase.h"
#include "Player/FHPlayerAnimInstance.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "DataAsset/FHPlayerAnimMontageDataAsset.h"
#include "Animation/AnimMontage.h"
#include "AbilitySystemComponent.h"

UFH_GA_PlayerSwitchingTools::UFH_GA_PlayerSwitchingTools()
{
	// ExecutionPolicy가 ServerOnly면 서버에선 GA가 최초로 한 번 호출되지만, 이 사실을 클라이언트는 알 수 없으며, 따라서 서버에서는 도구를 바꾸었지만 클라의 화면에선 아무 일도 일어나지 않게 된다. 따라서 서버가 먼저 수행하고 그걸 클라로 전파하고 싶은 거라면 ServerInitiated를 사용해야 한다.
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	// 막지마?
	//ActivationOwnedTags.AddTag(GET_GAMEPLAY_TAG_PLAYER_STATE_ISACTIONOCCUPIED);
}

void UFH_GA_PlayerSwitchingTools::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	Player = Cast<AFHPlayerBase>(ActorInfo->AvatarActor.Get());
	if (!Player)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	AnimInstance = Cast<UFHPlayerAnimInstance>(Player->GetMesh()->GetAnimInstance());
	if (!AnimInstance)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}
	
	if (!TriggerEventData || TriggerEventData->TargetData.Num() == 0)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	const FFHGACustomData_SwitchingType* CustomData = static_cast<const FFHGACustomData_SwitchingType*>(TriggerEventData->TargetData.Get(0));
	if (CustomData == nullptr)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	//AnimInstance->OnMontageEnded.AddDynamic(this, &UFH_GA_PlayerSwitchingTools::OnMontageEnd);

	Player->PendingMesh = CustomData->PendingMesh_Ready;
	Player->PendingType = CustomData->ToType;

	PlayMontage(CustomData->FromType, CustomData->ToType);

	UAbilityTask_WaitDelay* delay = UAbilityTask_WaitDelay::WaitDelay(this, AbilityDuration);
	delay->OnFinish.AddDynamic(this, &UFH_GA_PlayerSwitchingTools::OnTimerSet);
	delay->ReadyForActivation();
}

void UFH_GA_PlayerSwitchingTools::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	
}

void UFH_GA_PlayerSwitchingTools::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (Player && bIsTerminalOpen == false)
	{
		Player->ForceUpdateCurrentEquipmentAndGrabMesh();
	}
	if (AnimInstance)
	{
		AnimInstance->OnMontageEnded.RemoveDynamic(this, &UFH_GA_PlayerSwitchingTools::OnMontageEnd);
	}

}

void UFH_GA_PlayerSwitchingTools::OnTimerSet()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UFH_GA_PlayerSwitchingTools::OnMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	if (Player)
	{
		if (Player->bIsTerminalOpen)
		{
			bIsTerminalOpen = true;
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
			return;
		}
	}


	if (bInterrupted)
	{
		
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void UFH_GA_PlayerSwitchingTools::PlayMontage(EItemHoldingType PrevType, EItemHoldingType NewType)
{
	if (!GetAbilitySystemComponentFromActorInfo()) return;
	FGameplayCueParameters params;

	// 임시로 손을 비우는 동작은 모든 인스턴스에서 각자 이루어져야 하므로 몽타쥬의 노티파이로 수행

	if (NewType == EItemHoldingType::OneHanded_Throw)
	{
		GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(MontageCueTag_0to1T, params);
		
	}
	else if (NewType == EItemHoldingType::OneHanded_Gun)
	{
		GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(MontageCueTag_0to1G, params);
	}
	else if (NewType == EItemHoldingType::TwoHanded)
	{
		GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(MontageCueTag_0to2, params);
	}
	else if (NewType == EItemHoldingType::NoHanded)
	{
		if (PrevType == EItemHoldingType::OneHanded_Throw)
		{
			GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(MontageCueTag_1Tto0, params);
		}
		else if (PrevType == EItemHoldingType::OneHanded_Gun)
		{
			GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(MontageCueTag_1Gto0, params);
		}
		else if (PrevType == EItemHoldingType::TwoHanded)
		{
			GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(MontageCueTag_2to0, params);
		}
	}
	AnimInstance->OnMontageEnded.AddDynamic(this, &UFH_GA_PlayerSwitchingTools::OnMontageEnd);
}