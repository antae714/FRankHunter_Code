// Copyright F Rank Hunter. All Rights Reserved.


#include "GAS/GameplayCue/FHPlayMontageCue.h"
#include "GameFramework/Character.h"
#include "Player/FHPlayerBase.h"
#include "Core/FHPlayerStateBase.h"

bool UFHPlayMontageCue::HandlesEvent(EGameplayCueEvent::Type EventType) const
{
	// only handle excuted event.
	return (EventType == EGameplayCueEvent::Executed);
}

void UFHPlayMontageCue::HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters)
{
	Super::HandleGameplayCue(MyTarget, EventType, Parameters);

	if (!IsValid(MyTarget)) return;

	if (bEnableBlending)
	{
		BlendingArg.BlendOption = EAlphaBlendOption::Linear;
		BlendingArg.BlendTime = BlendTime;
	}
	else
	{
		BlendingArg.BlendTime = 0.0f;
	}

	ACharacter* Chara = Cast<ACharacter>(MyTarget);
	if (Chara == nullptr) return;

	UAnimInstance* AnimInstance = Chara->GetMesh()->GetAnimInstance();
	if (AnimInstance == nullptr) return;

	if (Target != EPlayMontageCueTarget::DiffrentLocalAndOthers)
	{
		if (Chara->IsLocallyControlled())
		{
			if (Target == EPlayMontageCueTarget::ExceptLocalPlayer) return;

		}
		else
		{
			if (Target == EPlayMontageCueTarget::OnlyLocalPlayer) return;

		}
		PlayMontage(AnimInstance, AnimMontage, SectionName, PlayRate);
	}
	else
	{
		if (Chara->IsLocallyControlled())
		{
			PlayMontage(AnimInstance, AnimMontageLocal, SectionNameLocal, PlayRateLocal);
		}
		else
		{
			PlayMontage(AnimInstance, AnimMontageOther, SectionNameOther, PlayRateOther);
		}
	}
}

void UFHPlayMontageCue::PlayMontage(UAnimInstance* _AnimInstance, UAnimMontage* _AnimMontage, FName _SectionName, float _PlayRate)
{
	if (bConsiderPlayerActionSpeed)
	{
		AFHPlayerBase* Player = Cast<AFHPlayerBase>(_AnimInstance->GetOwningActor());
		_PlayRate *= Player->GetActionSpeedMult();
	}

	if (Mode == EPlayMontageCueMode::Play)
	{
		if (_AnimInstance->IsAnyMontagePlaying())
		{
			if (bEnableBlending)
			{
				_AnimInstance->StopAllMontages(BlendTime);
			}
			else
			{
				_AnimInstance->StopAllMontages(0.0f);
			}
		}


		if (bEnableBlending)
		{
			_AnimInstance->Montage_PlayWithBlendIn(_AnimMontage, BlendingArg, _PlayRate);
			_AnimInstance->Montage_JumpToSection(_SectionName);
		}
		else
		{
			_AnimInstance->Montage_Play(_AnimMontage, _PlayRate);
			_AnimInstance->Montage_JumpToSection(_SectionName);
		}
	}
	else if (Mode == EPlayMontageCueMode::Jump)
	{
		_AnimInstance->Montage_SetPlayRate(_AnimMontage, _PlayRate);
		_AnimInstance->Montage_JumpToSection(_SectionName);
	}
}
