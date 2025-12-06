// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FHDoorBase.generated.h"

class UInputAction;


UCLASS()
class FRANKHUNTER_API AFHDoorBase : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<class USceneComponent> DoorRoot;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UStaticMeshComponent> DoorMesh;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UPhysicsConstraintComponent> PhysicsConstraint;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UFHInteractableComponent> InteractableComp;

	UPROPERTY(EditAnywhere)
	float MotorStrengthWhenOpenForcefully{ 100.0f };
	UPROPERTY(EditAnywhere)
	float MotorStrengthWhenOpenSlowly{ 30.0f };
	UPROPERTY(EditAnywhere)
	float OpenDegree{ 90.0f };
	UPROPERTY(EditAnywhere)
	float RotationLimit{ 100.0f };

	UPROPERTY(EditAnywhere)
	uint32 bHingeIsRightOnFrontView : 1{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AFHDoorBase> PairDoor;


	/*UPROPERTY(EditAnywhere)
	float TorquePerDegree{ 20.0f };
	UPROPERTY(EditAnywhere)
	float ClosedAngleThreshold{ 10.0f };*/


protected:
	float initialYaw{ 0.0f };


public:
	AFHDoorBase();

protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

public:
	virtual void Tick(float DeltaTime) override;

	class UPhysicsConstraintComponent* GetPhysicsConstraint(){ return PhysicsConstraint; }
	class UStaticMeshComponent* GetStaticMesh(){ return DoorMesh; }

	UFUNCTION(BlueprintCallable)
	void OpenDoorForcefully(AActor* Subject = nullptr, float ForceMult = 7.0f);

	bool IsOpened() { return bIsOpened; }
private:
	bool bIsOpened{ false };
	bool bPushDoorSlowly{ false };

	// float GetReverseTorqueDirection(class AFHPlayerBase* Player);
	// bool IsDoorClosed();
	// float GetDeltaAngleFromInitialDegree();
	bool IsDoorOpenDirectionForward(AActor* Subject);

protected:
	UFUNCTION(NetMulticast, Reliable)
	void ToggleDoorOpen(AActor* Subject, float power, bool alsoHandlePair = true);

	UFUNCTION()
	void OnInteract_Impl(class AFHPlayerBase* Player, const UInputAction* InputAction);
	UFUNCTION()
	void OnInteractHold_Impl(class AFHPlayerBase* Player, const UInputAction* InputAction, float HoldDuration);
	UFUNCTION()
	void OnInteractRelease_Impl(class AFHPlayerBase* Player, const UInputAction* InputAction, float HoldDuration);



	void HandleDoor(class UStaticMeshComponent* DoorMesh, class UPhysicsConstraintComponent* PhysicsConstraint, FRotator AngularOrientation, float movePower);
};
