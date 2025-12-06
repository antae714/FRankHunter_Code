// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/FHGameplayTags.h"
#include "FHCharacterMovementComponent.generated.h"

UENUM(BlueprintType)
enum ECustomMovementMode : int
{
	CMOVE_NONE		UMETA(DisplayName = "None"),
	CMOVE_CartDrive UMETA(DisplayName = "Cart Drive"),
};

UCLASS()
class FRANKHUNTER_API UFHCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
	// for movement replication
	class FFHSavedMove : public FSavedMove_Character
	{
	public:

		typedef FSavedMove_Character Super;

		///@brief Resets all saved variables.
		virtual void Clear() override;

		///@brief Store input commands in the compressed flags.
		virtual uint8 GetCompressedFlags() const override;

		///@brief This is used to check whether or not two moves can be combined into one.
		///Basically you just check to make sure that the saved variables are the same.
		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const override;

		///@brief Sets up the move before sending it to the server. 
		virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData) override;
		///@brief Sets variables on character movement component before making a predictive correction.
		virtual void PrepMoveFor(class ACharacter* Character) override;

		// Sprint
		uint8 SavedRequestToStartSprinting : 1;
	};

	class FFHNetworkPredictionData_Client : public FNetworkPredictionData_Client_Character
	{
	public:
		FFHNetworkPredictionData_Client(const UCharacterMovementComponent& ClientMovement);

		typedef FNetworkPredictionData_Client_Character Super;

		///@brief Allocates a new copy of our custom saved move
		virtual FSavedMovePtr AllocateNewMove() override;
	};



public:
	// RunSpeed Attribute overrides it.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement: Sprinting")
	float SprintSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Custom")
	float MinimumJumpVelocityThreshold{ 200.0f };

	uint8 RequestToStartSprinting : 1;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: CartDrive")
	float CartDriveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: CartDrive")
	float CartDriveMaxSteerAngle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: CartDrive")
	float CartDriveSteerThreathHold;

public:
	UFHCharacterMovementComponent();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual float GetMaxSpeed() const override;
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;

	virtual bool CanAttemptJump() const override;

	virtual void SetMovementMode(EMovementMode NewMovementMode, uint8 NewCustomMode = 0) override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	virtual bool CheckFall(const FFindFloorResult& OldFloor, const FHitResult& Hit, const FVector& Delta, const FVector& OldLocation, float remainingTime, float timeTick, int32 Iterations, bool bMustJump) override;
	
	// Sprint
	UFUNCTION(BlueprintCallable, Category = "Sprint")
	void StartSprinting();
	UFUNCTION(BlueprintCallable, Category = "Sprint")
	void StopSprinting();


	bool IsSprinting() const;
	bool IsCartDriving() const;
	
};
