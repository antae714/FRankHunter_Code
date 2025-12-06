// Copyright F Rank Hunter. All Rights Reserved.


#include "GAS/Ability/FH_GA_PlayerPunchAttack.h"

#include "FRankHunter.h"
#include "GAS/FHGameplayTags.h"
#include "Player/FHPlayerBase.h"
#include "Item\FHInventoryComponent.h"
#include "BluePrintFunctions\FHBlueprintFunctionLibrary.h"
#include "Item/FHItemBase.h"
#include "Player/FHPlayerAnimInstance.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "DataAsset/FHPlayerAnimMontageDataAsset.h"
#include "Animation/AnimMontage.h"
#include "AbilitySystemComponent.h"

UFH_GA_PlayerPunchAttack::UFH_GA_PlayerPunchAttack()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UFH_GA_PlayerPunchAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	Player = Cast<AFHPlayerBase>(ActorInfo->AvatarActor.Get());
	if (!Player)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	UAnimMontage* PunchMontage = Player->PlayerDescriptor->AnimMontages->PlayerPunch;
	FName StartSection{ TEXT("Start") };
	if (!PunchMontage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayerPunch"), PunchMontage, 1.0f, StartSection);
	MontageTask->OnCompleted.AddDynamic(this, &UFH_GA_PlayerPunchAttack::OnMontageEnd);
	MontageTask->OnInterrupted.AddDynamic(this, &UFH_GA_PlayerPunchAttack::OnMontageEnd);

	MontageTask->ReadyForActivation();

	// Sound only
	if (GetAbilitySystemComponentFromActorInfo() && MontageCueTag_Punch.IsValid())
	{
		FGameplayCueParameters params;
		GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(MontageCueTag_Punch, params);
	}

	// ¸ùÅ¸Áê ¾ÃÇô¼­ ¾È ³¡³ª´Â »çÅÂ ´ëºñ
	UAbilityTask_WaitDelay* delay = UAbilityTask_WaitDelay::WaitDelay(this, AbilityDuration);
	delay->OnFinish.AddDynamic(this, &UFH_GA_PlayerPunchAttack::OnTimerSet);
	delay->ReadyForActivation();
}

void UFH_GA_PlayerPunchAttack::OnMontageEnd()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UFH_GA_PlayerPunchAttack::OnTimerSet()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
