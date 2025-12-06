// Copyright F Rank Hunter. All Rights Reserved.


#include "GAS/Ability/FH_GA_PlayerKnockdown.h"
#include "AbilitySystemComponent.h"
#include "Player/FHPlayerBase.h"
#include "Component/FHCharacterMovementComponent.h"
#include "GameplayEffect.h"
#include "GAS/FHGameplayAbilityTargetDatas.h"
#include "DrawDebugHelpers.h"


#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "GAS/AbilityTask/FHAbilityTask_WaitMeshDontMove.h"

UFH_GA_PlayerKnockdown::UFH_GA_PlayerKnockdown()
{
	// 클라이언트가 ServerRPC를 호출하고, 거기서 GA를 발동하는 경우, LocalPredicted로 되어있으면 GA를 발동시키는 ASC가 Autonomous Proxy가 아니라 서버에 있는 Simulated Proxy라 아예 발동이 안 되는 걸로 보인다.
	// 따라서 ServerOnly로 실행. 래그돌화나 Impulse를 가하는 건 멀티캐스트 되므로 상관 없다.
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UFH_GA_PlayerKnockdown::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AFHPlayerBase* Player = Cast<AFHPlayerBase>(ActorInfo->AvatarActor);
	if (!Player)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilitySystemComponent* asc = GetAbilitySystemComponentFromActorInfo();
	if (!asc)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	const FFHGACustomData_ThrowingPlayer* CustomData = static_cast<const FFHGACustomData_ThrowingPlayer*>(TriggerEventData->TargetData.Get(0));
	if (CustomData == nullptr)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}
	
	ThrowDirection = CustomData->ThrowDirection.GetSafeNormal();
	ThrowPower = CustomData->ThrowPower;
	ImpulseTargetBone = CustomData->Targetbone;
	isDead = CustomData->bIsDead;

	// 문제1: SetSimulatePhysics(true)를 호출한다고 그 틱에서 바로 바뀌는게 아님 -> 같은 틱에서 AddImpulse를 호출해봐야 적용이 안 됨.
	// 대안: 그냥 MovementComponent의 Velocity를 갱신해버리고 래그돌로 만들기

	Player->Server_SetRagdollingState(true);

	FGameplayEffectSpecHandle EffectHandle = asc->MakeOutgoingSpec(KnockdownEffect, 1, asc->MakeEffectContext());
	asc->ApplyGameplayEffectSpecToSelf(*EffectHandle.Data.Get());

	// 다음 틱에만 하면 됨 ~ 0.0초 대기
	UAbilityTask_WaitDelay* timer = UAbilityTask_WaitDelay::WaitDelay(this, 0.0f);
	timer->OnFinish.AddDynamic(this, &UFH_GA_PlayerKnockdown::ThrowItAway);
	timer->ReadyForActivation();
}

void UFH_GA_PlayerKnockdown::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
}

void UFH_GA_PlayerKnockdown::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UFH_GA_PlayerKnockdown::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UFH_GA_PlayerKnockdown::ThrowItAway()
{
	AFHPlayerBase* Player = Cast<AFHPlayerBase>(GetCurrentActorInfo()->AvatarActor);
	Player->ImpulseToPlayer(ThrowDirection, ThrowPower, ImpulseTargetBone);

	if (isDead == false)
	{
		UFHAbilityTask_WaitMeshDontMove* Task = UFHAbilityTask_WaitMeshDontMove::WaitForMeshStop(this, Player->GetMesh(), 10.0f, ImpulseTargetBone, 0.5f);
		Task->OnMeshDontMove.AddDynamic(this, &UFH_GA_PlayerKnockdown::WakeUp);
		Task->ReadyForActivation();
	}
	else
	{
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
	}
}

void UFH_GA_PlayerKnockdown::WakeUp()
{
	AFHPlayerBase* Player = Cast<AFHPlayerBase>(GetCurrentActorInfo()->AvatarActor);

	Player->Server_RecoverFromRagdolling();

	UAbilityTask_WaitDelay* timer = UAbilityTask_WaitDelay::WaitDelay(this, 1.35f);
	timer->OnFinish.AddDynamic(this, &UFH_GA_PlayerKnockdown::AfterWakeUp);
	timer->ReadyForActivation();
}

void UFH_GA_PlayerKnockdown::AfterWakeUp()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}
