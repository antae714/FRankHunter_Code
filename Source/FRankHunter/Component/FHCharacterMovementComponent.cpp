// Copyright F Rank Hunter. All Rights Reserved.


#include "Component/FHCharacterMovementComponent.h"
#include "FRankHunter.h"

#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "GAS/FHAttributeSet_Stamina.h"

void UFHCharacterMovementComponent::FFHSavedMove::Clear()
{
	Super::Clear();

	SavedRequestToStartSprinting = false;
}

uint8 UFHCharacterMovementComponent::FFHSavedMove::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

	if (SavedRequestToStartSprinting)
	{
		Result |= FLAG_Custom_0;
	}

	return Result;
}

bool UFHCharacterMovementComponent::FFHSavedMove::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const
{
	//Set which moves can be combined together. This will depend on the bit flags that are used.
	if (SavedRequestToStartSprinting != ((FFHSavedMove*)&NewMove)->SavedRequestToStartSprinting)
	{
		return false;
	}

	return Super::CanCombineWith(NewMove, Character, MaxDelta);
}

void UFHCharacterMovementComponent::FFHSavedMove::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	UFHCharacterMovementComponent* CharacterMovement = Cast<UFHCharacterMovementComponent>(Character->GetCharacterMovement());
	if (CharacterMovement)
	{
		SavedRequestToStartSprinting = CharacterMovement->RequestToStartSprinting;
	}
}

void UFHCharacterMovementComponent::FFHSavedMove::PrepMoveFor(class ACharacter* Character)
{
	Super::PrepMoveFor(Character);

	UFHCharacterMovementComponent* CharacterMovement = Cast<UFHCharacterMovementComponent>(Character->GetCharacterMovement());
	if (CharacterMovement)
	{
	}
}

UFHCharacterMovementComponent::FFHNetworkPredictionData_Client::FFHNetworkPredictionData_Client(const UCharacterMovementComponent& ClientMovement)
	: Super(ClientMovement)
{

}

FSavedMovePtr UFHCharacterMovementComponent::FFHNetworkPredictionData_Client::AllocateNewMove()
{
	return FSavedMovePtr(new FFHSavedMove());
}

UFHCharacterMovementComponent::UFHCharacterMovementComponent()
{
	SprintSpeed = 400.0f;
	MaxCustomMovementSpeed = MaxWalkSpeed;

	CartDriveSpeed = MaxWalkSpeed;
	CartDriveSteerThreathHold = 0.5f;
	CartDriveMaxSteerAngle = 30.0f;
}

void UFHCharacterMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

float UFHCharacterMovementComponent::GetMaxSpeed() const
{
	if (RequestToStartSprinting)
	{
		return SprintSpeed;
	}
	
	return Super::GetMaxSpeed();
}

void UFHCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	RequestToStartSprinting = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
}

class FNetworkPredictionData_Client* UFHCharacterMovementComponent::GetPredictionData_Client() const
{
	//return Super::GetPredictionData_Client();
	

	check(PawnOwner != NULL);

	if (!ClientPredictionData)
	{
		UFHCharacterMovementComponent* MutableThis = const_cast<UFHCharacterMovementComponent*>(this);

		MutableThis->ClientPredictionData = new FFHNetworkPredictionData_Client(*this);
		//MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
		//MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
	}

	return ClientPredictionData;
}

bool UFHCharacterMovementComponent::CanAttemptJump() const
{
	if (!Super::CanAttemptJump())
	{
		return false;
	}

	if (IsCartDriving())
	{
		return false;
	}

	if (JumpZVelocity < MinimumJumpVelocityThreshold)
	{
		return false;
	}
	return true;
}

void UFHCharacterMovementComponent::SetMovementMode(EMovementMode NewMovementMode, uint8 NewCustomMode)
{
	if (NewMovementMode == MOVE_Falling && NewCustomMode == 0)
	{
		// Prevent falling if the player is not allowed to jump
		if (!CanAttemptJump())
		{
			NewMovementMode = MOVE_Walking;
			NewCustomMode = 0;
		}
	}

	Super::SetMovementMode(NewMovementMode, NewCustomMode);

	if (NewCustomMode == CMOVE_CartDrive)
	{
		//MaxCustomMovementSpeed = CartDriveSpeed;
	}
}

void UFHCharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	Super::PhysCustom(deltaTime, Iterations);

	if (CustomMovementMode == CMOVE_CartDrive)
	{
		const FVector Forward = GetForwardVector();
		const float Speed = FVector::DotProduct(Acceleration, Forward);
		float SteeringInput = 0.f;
		//if (FMath::Abs(Speed) > 1)
		{
			FVector AccelerationDir = Acceleration.GetSafeNormal();
			SteeringInput = FVector::DotProduct(AccelerationDir, GetOwner()->GetActorRightVector());
		}

		//const float DeltaYaw = FMath::Min(SteeringInput * deltaTime * CartDriveSteerThreathHold, CartDriveMaxSteerAngle);
		const float DeltaYaw = SteeringInput * deltaTime * CartDriveSteerThreathHold;
		if (GetOwner())
		{
			GetOwner()->AddActorWorldRotation(FRotator(0.f, DeltaYaw, 0.f));
		}

		//Velocity = GetForwardVector() * Speed;
		Acceleration = FVector::ZeroVector;
		//Velocity = FVector::ZeroVector;
		//PhysWalking(deltaTime, Iterations);
	}
}

bool UFHCharacterMovementComponent::CheckFall(const FFindFloorResult& OldFloor, const FHitResult& Hit, const FVector& Delta, const FVector& OldLocation, float remainingTime, float timeTick, int32 Iterations, bool bMustJump)
{
	bool bIsFalling = Super::CheckFall(OldFloor, Hit, Delta, OldLocation, remainingTime, timeTick, Iterations, bMustJump);
	if (IsCartDriving())
	{
		return false;
	}
	else
	{
		return bIsFalling;
	}
}

void UFHCharacterMovementComponent::StartSprinting()
{
	RequestToStartSprinting = true;
}

void UFHCharacterMovementComponent::StopSprinting()
{
	RequestToStartSprinting = false;
}

bool UFHCharacterMovementComponent::IsSprinting() const
{
	return static_cast<bool>(RequestToStartSprinting);
}

bool UFHCharacterMovementComponent::IsCartDriving() const
{
	return CustomMovementMode == CMOVE_CartDrive;
}
