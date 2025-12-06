// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "ChaosVehicleWheel.h"
#include "Interfaces\SiInventorySystemInterface.h"
#include "FHCart.generated.h"

class UChaosWheeledVehicleMovementComponent;
class UFHInteractableComponent;
class UFHInventoryComponent;
class UInventoryBase;
class USpringArmComponent;
class AFHPlayerBase;
class UInputAction;

struct FInputActionValue;
/**
 * 
 */
UCLASS()
class FRANKHUNTER_API AFHCart : public AWheeledVehiclePawn, public ISiInventorySystemInterface
{
	GENERATED_BODY()

	/** Chassis static mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Meshes, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> Chassis;

	///** FL Tire static mesh */
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Meshes, meta = (AllowPrivateAccess = "true"))
	//TObjectPtr<UStaticMeshComponent> TireFrontLeft;

	///** FR Tire static mesh */
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Meshes, meta = (AllowPrivateAccess = "true"))
	//TObjectPtr<UStaticMeshComponent> TireFrontRight;

	///** RL Tire static mesh */
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Meshes, meta = (AllowPrivateAccess = "true"))
	//TObjectPtr<UStaticMeshComponent> TireRearLeft;

	///** RR Tire static mesh */
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Meshes, meta = (AllowPrivateAccess = "true"))
	//TObjectPtr<UStaticMeshComponent> TireRearRight;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;



	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UFHInteractableComponent> InteractableComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UFHInventoryComponent> InventoryComponent;
public:
	AFHCart();

public:
	UPROPERTY(EditAnywhere, Category = Widget)
	TSubclassOf<UInventoryBase> CartUIClass;

	UPROPERTY()
	TObjectPtr<UInventoryBase> CartUIInstance;
	uint8 bIsWidgetPopup : 1;


public:
	void Move(const FInputActionValue& Value);

	
	TObjectPtr<UChaosWheeledVehicleMovementComponent> GetChaosVehicleMovement() const;
	virtual USiInventoryComponent* GetInventoryComponent() const override;

	void ToggleDrivePlayer(AFHPlayerBase* Player);
	bool IsFallOver() const;

	void SettingGameInstance();

	void LateInit();
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
private:

	UFUNCTION()
	void OnInteract_Impl(AFHPlayerBase* Player, const UInputAction* InputAction);

	UFUNCTION()
	void OnInteractHold_Impl(AFHPlayerBase* Player, const UInputAction* InputAction, float HoldDuration);

	UFUNCTION()
	void OnInteractRelease_Impl(AFHPlayerBase* Player, const UInputAction* InputAction, float HoldDuration);

	bool isInteract;
	bool isInteractHold;
};

/**
 *  Front wheel definition for Offroad Car.
 */
UCLASS()
class UCartVihecleWheelFront : public UChaosVehicleWheel
{
	GENERATED_BODY()

public:
	UCartVihecleWheelFront();
};

/**
 *  Rear wheel definition for Offroad Car.
 */
UCLASS()
class UCartVihecleWheelRear : public UChaosVehicleWheel
{
	GENERATED_BODY()

public:

	UCartVihecleWheelRear();
};
