// Copyright F Rank Hunter. All Rights Reserved.


#include "GAS/Attributes/FHAttributeSet_Movement.h"
#include "Net/UnrealNetwork.h"
#include "FRankHunter.h"

#include "Gameframework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Component/FHCharacterMovementComponent.h"

void UFHAttributeSet_Movement::OnRep_WalkSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFHAttributeSet_Movement, WalkSpeed, OldValue);

	ACharacter* avatar = Cast<ACharacter>(GetActorInfo()->AvatarActor.Get());
	if (!avatar) return;

	UFHCharacterMovementComponent* moveComp = Cast<UFHCharacterMovementComponent>(avatar->GetCharacterMovement());
	if (!moveComp) return;

	moveComp->MaxWalkSpeed = GetWalkSpeed();
	moveComp->MaxWalkSpeedCrouched = GetWalkSpeed() * 0.5f;
}

void UFHAttributeSet_Movement::OnRep_RunSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFHAttributeSet_Movement, RunSpeed, OldValue);

	ACharacter* avatar = Cast<ACharacter>(GetActorInfo()->AvatarActor.Get());
	if (!avatar) return;

	UFHCharacterMovementComponent* moveComp = Cast<UFHCharacterMovementComponent>(avatar->GetCharacterMovement());
	if (!moveComp) return;

	moveComp->SprintSpeed = GetRunSpeed();
}

void UFHAttributeSet_Movement::OnRep_JumpPower(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFHAttributeSet_Movement, WalkSpeed, OldValue);

	ACharacter* avatar = Cast<ACharacter>(GetActorInfo()->AvatarActor.Get());
	if (!avatar) return;

	UFHCharacterMovementComponent* moveComp = Cast<UFHCharacterMovementComponent>(avatar->GetCharacterMovement());
	if (!moveComp) return;

	moveComp->JumpZVelocity = GetJumpPower();
}

void UFHAttributeSet_Movement::OnRep_ActionSpeedMult(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFHAttributeSet_Movement, ActionSpeedMult, OldValue);

	ACharacter* avatar = Cast<ACharacter>(GetActorInfo()->AvatarActor.Get());
	if (!avatar) return;

	USkeletalMeshComponent* skc = avatar->GetMesh();
	if (!skc) return;

	UAnimInstance* ai = skc->GetAnimInstance();
	if (!ai) return;

	float changedRate = GetActionSpeedMult() / OldValue.GetCurrentValue();
	if (ai->IsAnyMontagePlaying())
	{
		UAnimMontage* montage = ai->GetCurrentActiveMontage();
		ai->Montage_SetPlayRate(montage, ai->Montage_GetPlayRate(montage) * changedRate);
	}
}

void UFHAttributeSet_Movement::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(UFHAttributeSet_Movement, WalkSpeed);
	DOREPLIFETIME(UFHAttributeSet_Movement, RunSpeed);
	DOREPLIFETIME(UFHAttributeSet_Movement, JumpPower);
	DOREPLIFETIME(UFHAttributeSet_Movement, ActionSpeedMult);
}

void UFHAttributeSet_Movement::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	if (Attribute == GetWalkSpeedAttribute())
	{
		ACharacter* owner = Cast<ACharacter>(GetOwningAbilitySystemComponent()->GetAvatarActor());
		if (!owner) return;
		UFHCharacterMovementComponent* moveComp = Cast<UFHCharacterMovementComponent>(owner->GetCharacterMovement());
		if (!moveComp) return;

		moveComp->MaxWalkSpeed = NewValue;
		moveComp->MaxWalkSpeedCrouched = NewValue * 0.5f;
	}
	else if (Attribute == GetRunSpeedAttribute())
	{
		ACharacter* owner = Cast<ACharacter>(GetOwningAbilitySystemComponent()->GetAvatarActor());
		if (!owner) return;
		UFHCharacterMovementComponent* moveComp = Cast<UFHCharacterMovementComponent>(owner->GetCharacterMovement());
		if (!moveComp) return;

		moveComp->SprintSpeed = NewValue;

	}
	else if (Attribute == GetJumpPowerAttribute())
	{
		ACharacter* owner = Cast<ACharacter>(GetOwningAbilitySystemComponent()->GetAvatarActor());
		if (!owner) return;
		UFHCharacterMovementComponent* moveComp = Cast<UFHCharacterMovementComponent>(owner->GetCharacterMovement());
		if (!moveComp) return;

		moveComp->JumpZVelocity = NewValue;
	}
	else if (Attribute == GetActionSpeedMultAttribute())
	{
		ACharacter* owner = Cast<ACharacter>(GetOwningAbilitySystemComponent()->GetAvatarActor());
		if (!owner) return;

		USkeletalMeshComponent* skc = owner->GetMesh();
		if (!skc) return;

		UAnimInstance* ai = skc->GetAnimInstance();
		if (!ai) return;

		float changedRate = NewValue / OldValue;
		if (ai->IsAnyMontagePlaying())
		{
			UAnimMontage* montage = ai->GetCurrentActiveMontage();
			ai->Montage_SetPlayRate(montage, ai->Montage_GetPlayRate(montage) * changedRate);
		}
	}
}

void UFHAttributeSet_Movement::OnInitAbilityActorInfo()
{
	OnRep_WalkSpeed(WalkSpeed);
	OnRep_RunSpeed(RunSpeed);
	OnRep_JumpPower(JumpPower);
	OnRep_ActionSpeedMult(ActionSpeedMult);
}
