// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "Common/CommonItemEnum.h"
#include "FHPlayerAnimInstance.generated.h"


DECLARE_DYNAMIC_DELEGATE(FOnStartCharge);

UCLASS()
class FRANKHUNTER_API UFHPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	FOnStartCharge OnStartChargeEvent;
	UFUNCTION(BlueprintCallable)
	void OnStartCharge(){ OnStartChargeEvent.ExecuteIfBound(); }

protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class AFHPlayerBase> Player;

	UPROPERTY()
	class UFHCharacterMovementComponent* MoveComp{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Direction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector Velocity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AimPitch;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint32 bIsClickNow : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint32 bIsAiming : 1; // 한 손 전용

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint32 bIsFalling : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint32 bIsSprinting : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint32 bIsCrouching : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint32 bIsMoving : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint32 bIsRagdolling : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint32 bGetUpFromFront : 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinimumSpeedToSetIsMoving{ 10.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LandingAnimationSkipThreshold{ 0.5f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EItemHoldingType HoldingType;
};
