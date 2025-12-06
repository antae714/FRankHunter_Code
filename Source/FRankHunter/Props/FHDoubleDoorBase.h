// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FHDoubleDoorBase.generated.h"

class UInputAction;

UCLASS()
class FRANKHUNTER_API AFHDoubleDoorBase : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UStaticMeshComponent> DoorFrameMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UStaticMeshComponent> LeftDoorMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UStaticMeshComponent> RightDoorMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UPhysicsConstraintComponent> LeftConstraint;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UPhysicsConstraintComponent> RightConstraint;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UFHInteractableComponent> InteractableComp;

	UPROPERTY(EditAnywhere)
	float MotorStrengthWhenOpenForcefully{ 100.0f };
	UPROPERTY(EditAnywhere)
	float MotorStrengthWhenOpenSlowly{ 30.0f };
	UPROPERTY(EditAnywhere)
	float RotationLimit{ 90.0f };

	UPROPERTY(EditAnywhere)
	uint32 bHingeIsRightOnFrontView : 1{ false };


public:
	AFHDoubleDoorBase();

protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

private:
	bool bIsOpened{ false };
	bool bPushDoorSlowly{ false };

	bool IsDoorOpenDirectionForward(class AFHPlayerBase* Player);

protected:
	UFUNCTION(NetMulticast, Reliable)
	void ToggleDoorOpen(class AFHPlayerBase* Player, bool slowly);

	UFUNCTION()
	void OnInteract_Impl(class AFHPlayerBase* Player, const UInputAction* InputAction);
	UFUNCTION()
	void OnInteractHold_Impl(class AFHPlayerBase* Player, const UInputAction* InputAction, float HoldDuration);
	UFUNCTION()
	void OnInteractRelease_Impl(class AFHPlayerBase* Player, const UInputAction* InputAction, float HoldDuration);

	void HandleDoor(class UStaticMeshComponent* DoorMesh, class UPhysicsConstraintComponent* PhysicsConstraint, FRotator AngularOrientation, float movePower);
};
