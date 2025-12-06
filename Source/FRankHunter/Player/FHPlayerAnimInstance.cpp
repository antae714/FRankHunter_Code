// Copyright F Rank Hunter. All Rights Reserved.

#include "Player/FHPlayerAnimInstance.h"
#include "FRankHunter.h"
#include "Net/UnrealNetwork.h"
#include "FHPlayerBase.h"
#include "Component/FHCharacterMovementComponent.h"
#include "KismetAnimationLibrary.h"

void UFHPlayerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	Player = Cast<AFHPlayerBase>(TryGetPawnOwner());
	ensureMsgf(Player, TEXT("Cannot find player pawn."));

	MoveComp = Cast<UFHCharacterMovementComponent>(Player->GetCharacterMovement());
	ensureMsgf(MoveComp, TEXT("Cannot find movement component."));
}

void UFHPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (MoveComp)
	{
		Velocity = MoveComp->Velocity;
		MovementSpeed = Velocity.Size2D();
		Direction = UKismetAnimationLibrary::CalculateDirection(Player->GetVelocity(), Player->GetActorRotation());

		bIsFalling = MoveComp->IsFalling();
		bIsSprinting = MoveComp->IsSprinting();
		bIsCrouching = MoveComp->IsCrouching();
		bIsMoving = MovementSpeed >= MinimumSpeedToSetIsMoving ? true : false;
		AimPitch = Player->GetAimPitch();
		bIsAiming = Player->GetIsAiming();
		bIsRagdolling = Player->IsRagdolling();
		bIsClickNow = Player->GetIsClicked();
		if (bIsRagdolling)
		{
			bGetUpFromFront = Player->IsGetUpFromFront();
		}

		HoldingType = Player->GetCurrentEquipmentHoldingType();
	}
}