// Copyright F Rank Hunter. All Rights Reserved.


#include "GAS/Ability/FH_GA_PlayerAttack.h"

#include "FRankHunter.h"
#include "GAS/FHGameplayTags.h"
#include "Player/FHPlayerBase.h"
#include "Item\FHInventoryComponent.h"
#include "BluePrintFunctions\FHBlueprintFunctionLibrary.h"
#include "Item/FHItemBase.h"
#include "Player/FHPlayerAnimInstance.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "DataAsset/FHPlayerAnimMontageDataAsset.h"
#include "Animation/AnimMontage.h"
#include "AbilitySystemComponent.h"


UFH_GA_PlayerAttack::UFH_GA_PlayerAttack()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UFH_GA_PlayerAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	PRINT_LOG(TEXT("Activate Attack Ability"));

	Player = Cast<AFHPlayerBase>(ActorInfo->AvatarActor.Get());
	if (!Player)
	{
		PRINT_LOG(TEXT("Failed to attack."));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	ContinuousAttack = false;
	onSwing = false;
	onCharge = false;
	isReleased = false;
	AnimInstance = Cast<UFHPlayerAnimInstance>(Player->GetMesh()->GetAnimInstance());
	FPMontage = Player->GetAnimMontages()->FirstPersonAttackAnimMontage;
	TPMontage = Player->GetAnimMontages()->ThirdPersonAttackAnimMontage;

	if (!AnimInstance)
	{
		PRINT_LOG(TEXT("Failed to attack."));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	if (IsLocallyControlled())
	{
		if (AnimInstance->IsAnyMontagePlaying())
		{
			AnimInstance->StopAllMontages(0.0f);
		}

		TWeakObjectPtr<UFH_GA_PlayerAttack> WeakThis{ this };
		GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([WeakThis]()
		{
			if (!WeakThis.IsValid())
			{
				return;
			}

			WeakThis->AnimInstance->OnStartChargeEvent.BindDynamic(WeakThis.Get(), &UFH_GA_PlayerAttack::OnStartCharge);
			WeakThis->AnimInstance->OnMontageEnded.AddDynamic(WeakThis.Get(), &UFH_GA_PlayerAttack::OnMontageEnd);
		}));

		AnimInstance->Montage_Play(FPMontage, FP_Ready_PlayRate);
		AnimInstance->Montage_JumpToSection(TEXT("Ready"), FPMontage);
	}

	if (IsLocallyControlled())
	{
		Player->Server_PlayMontage(TPMontage, TEXT("Ready"), TP_Ready_PlayRate, true);
	}

	// sound only
	if (GetAbilitySystemComponentFromActorInfo() && MontageCueTag_Ready.IsValid())
	{
		FGameplayCueParameters params;
		GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(MontageCueTag_Ready, params);
	}
}

void UFH_GA_PlayerAttack::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UFH_GA_PlayerAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	//UseSucess();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	AnimInstance->OnStartChargeEvent.Clear();
	AnimInstance->OnMontageEnded.RemoveDynamic(this, &UFH_GA_PlayerAttack::OnMontageEnd);

	// º¸Çè
	if (AnimInstance->Montage_IsPlaying(FPMontage))
	{
		AnimInstance->Montage_Stop(0.2f, FPMontage);
	}
	Player->Server_PlayMontage(nullptr, {}, 1.0f, true);

	/*if (GetAbilitySystemComponentFromActorInfo() && MontageCueTag_CancelMontage.IsValid())
	{
		FGameplayCueParameters params;
		GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(MontageCueTag_CancelMontage, params);
	}*/
}

void UFH_GA_PlayerAttack::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);

	if (onSwing && ContinuousAttack == false)
	{
		ContinuousAttack = true;
	}
}

void UFH_GA_PlayerAttack::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	if (onSwing && ContinuousAttack == true)
	{
		ContinuousAttack = false;
	}

	if (onSwing || isReleased) return;

	isReleased = true;
	if (onCharge)
	{
		Swing();
	}
}

void UFH_GA_PlayerAttack::OnMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	if (ContinuousAttack)
	{
		ContinuousAttack = false;
		onCharge = false;
		isReleased = false;
		onSwing = false;

		AnimInstance->Montage_Play(FPMontage, FP_Ready_PlayRate);
		AnimInstance->Montage_JumpToSection(TEXT("Ready"), FPMontage);

		if (IsLocallyControlled())
		{
			Player->Server_PlayMontage(TPMontage, TEXT("Ready"), TP_Ready_PlayRate, true);
		}

		/*if (GetAbilitySystemComponentFromActorInfo() && MontageCueTag_Ready.IsValid())
		{
			FGameplayCueParameters params;
			GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(MontageCueTag_Ready_Loop, params);
		}*/
		return;
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

void UFH_GA_PlayerAttack::OnStartCharge()
{
	onCharge = true;
	if (isReleased == true)
	{
		Swing();
	}
}

void UFH_GA_PlayerAttack::Swing()
{
	onSwing = true;
	AnimInstance->Montage_SetPlayRate(FPMontage, FP_Swing_PlayRate);
	AnimInstance->Montage_JumpToSection(TEXT("Swing"), FPMontage);

	Player->Server_PlayMontage(TPMontage, TEXT("Swing"), TP_Swing_PlayRate, true);
	//Server_Swing();

	// sound only
	if (GetAbilitySystemComponentFromActorInfo() && MontageCueTag_Swing.IsValid())
	{
		FGameplayCueParameters params;
		GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(MontageCueTag_Swing, params);
	}
}

//void UFH_GA_PlayerAttack::Server_Swing_Implementation()
//{
//	if (GetAbilitySystemComponentFromActorInfo() && MontageCueTag_Swing.IsValid())
//	{
//		FGameplayCueParameters params;
//		GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(MontageCueTag_Swing, params);
//	}
//}