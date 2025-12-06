// Copyright F Rank Hunter. All Rights Reserved.


#include "Props/FHCart.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Component/FHInteractableComponent.h"
#include "Player/FHPlayerBase.h"
#include "EnhancedInputComponent.h"
#include "Item/FHInventoryComponent.h"
#include "UI/Inventory/InventoryBase.h"
#include "GameFramework/SpringArmComponent.h"

#include "Core/FHPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Lobby/FHShopBase.h"
#include "Core/FHUIManager.h"

AFHCart::AFHCart()
{
	// construct the mesh components
	Chassis = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Chassis"));
	Chassis->SetupAttachment(GetMesh());

	// Note: for faster iteration times, the vehicle setup can be tweaked in the Blueprint instead

	// Set up the chassis
	GetChaosVehicleMovement()->ChassisHeight = 160.0f;
	GetChaosVehicleMovement()->DragCoefficient = 0.1f;
	GetChaosVehicleMovement()->DownforceCoefficient = 0.1f;
	GetChaosVehicleMovement()->CenterOfMassOverride = FVector(0.0f, 0.0f, 75.0f);
	GetChaosVehicleMovement()->bEnableCenterOfMassOverride = true;

	// Set up the wheels
	GetChaosVehicleMovement()->bLegacyWheelFrictionPosition = true;
	GetChaosVehicleMovement()->WheelSetups.SetNum(4);

	GetChaosVehicleMovement()->WheelSetups[0].WheelClass = UCartVihecleWheelFront::StaticClass();
	GetChaosVehicleMovement()->WheelSetups[0].BoneName = FName("PhysWheel_FL");
	GetChaosVehicleMovement()->WheelSetups[0].AdditionalOffset = FVector(0.0f, 0.0f, 0.0f);

	GetChaosVehicleMovement()->WheelSetups[1].WheelClass = UCartVihecleWheelFront::StaticClass();
	GetChaosVehicleMovement()->WheelSetups[1].BoneName = FName("PhysWheel_FR");
	GetChaosVehicleMovement()->WheelSetups[1].AdditionalOffset = FVector(0.0f, 0.0f, 0.0f);

	GetChaosVehicleMovement()->WheelSetups[2].WheelClass = UCartVihecleWheelRear::StaticClass();
	GetChaosVehicleMovement()->WheelSetups[2].BoneName = FName("PhysWheel_BL");
	GetChaosVehicleMovement()->WheelSetups[2].AdditionalOffset = FVector(0.0f, 0.0f, 0.0f);

	GetChaosVehicleMovement()->WheelSetups[3].WheelClass = UCartVihecleWheelRear::StaticClass();
	GetChaosVehicleMovement()->WheelSetups[3].BoneName = FName("PhysWheel_BR");
	GetChaosVehicleMovement()->WheelSetups[3].AdditionalOffset = FVector(0.0f, 0.0f, 0.0f);

	// Set up the engine
	// NOTE: Check the Blueprint asset for the Torque Curve
	GetChaosVehicleMovement()->EngineSetup.MaxTorque = 600.0f;
	GetChaosVehicleMovement()->EngineSetup.MaxRPM = 5000.0f;
	GetChaosVehicleMovement()->EngineSetup.EngineIdleRPM = 1200.0f;
	GetChaosVehicleMovement()->EngineSetup.EngineBrakeEffect = 0.05f;
	GetChaosVehicleMovement()->EngineSetup.EngineRevUpMOI = 5.0f;
	GetChaosVehicleMovement()->EngineSetup.EngineRevDownRate = 600.0f;

	// Set up the differential
	GetChaosVehicleMovement()->DifferentialSetup.DifferentialType = EVehicleDifferential::AllWheelDrive;
	GetChaosVehicleMovement()->DifferentialSetup.FrontRearSplit = 0.5f;

	// Set up the steering
	// NOTE: Check the Blueprint asset for the Steering Curve
	GetChaosVehicleMovement()->SteeringSetup.SteeringType = ESteeringType::AngleRatio;
	GetChaosVehicleMovement()->SteeringSetup.AngleRatio = 0.7f;

	//GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName(FName("Vehicle"));
	GetMesh()->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	GetMesh()->SetPhysicsLinearVelocity(FVector::ZeroVector);


	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());


	InteractableComp = CreateDefaultSubobject<UFHInteractableComponent>(TEXT("InteractableComp"));

	InventoryComponent = CreateDefaultSubobject<UFHInventoryComponent>(TEXT("InventoryComponent"));
	InventoryComponent->SetMaxItemCountBeforePlay(4);
}

void AFHCart::BeginPlay()
{
	Super::BeginPlay();


	InteractableComp->OnInteractClient.AddDynamic(this, &ThisClass::OnInteract_Impl);
	InteractableComp->OnInteractHoldClient.AddDynamic(this, &ThisClass::OnInteractHold_Impl);
	InteractableComp->OnInteractReleaseClient.AddDynamic(this, &ThisClass::OnInteractRelease_Impl);

	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AFHCart::LateInit);
}

void AFHCart::LateInit()
{
	//AFHPlayerController* PC = Cast<AFHPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	//if (PC && PC->IsLocalController())
	//{
	//	if (PC->UIManager)
	//	{
	//		UFHShopBase* ShopUI = PC->UIManager->GetWidget<UFHShopBase>(TEXT("Shop"));
	//		if (ShopUI)
	//		{
	//			UE_LOG(LogTemp, Warning, TEXT("ShopUI Added."));
	//			//ShopUI->CartInventoryUI->SetInventoryComponent(GetComponentByClass<UFHInventoryComponent>());
	//			ShopUI->CartInventoryUI->SetInventoryComponent(InventoryComponent);
	//			UE_LOG(LogTemp, Warning, TEXT("Cart InventorySize: %d"), ShopUI->CartInventoryUI->InventorySize);
	//		}
	//	}
	//}
}

void AFHCart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetInstigator())
	{
		if(IsFallOver())
		{
			ToggleDrivePlayer(GetInstigator<AFHPlayerBase>());
		}
		else
		{
			APlayerController* PlayerController = GetInstigator()->GetController<APlayerController>();
			if (GetInstigator<AFHPlayerBase>() && PlayerController)
			{
				APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
				if (CameraManager)
				{
					CameraManager->ViewYawMin = GetActorRotation().Yaw - 30.0f;
					CameraManager->ViewYawMax = GetActorRotation().Yaw + 30.0f;
				}
				GetChaosVehicleMovement()->EngineSetup.MaxRPM = GetInstigator()->GetMovementComponent()->Velocity.Length() * 0.8f;
			}
		}


	}
}

void AFHCart::PostInitializeComponents()
{
	Super::PostInitializeComponents();


	GetMesh()->SetSimulatePhysics(true);
}

void AFHCart::OnInteract_Impl(AFHPlayerBase* Player, const UInputAction* InputAction)
{
	isInteract = true;
}

void AFHCart::OnInteractHold_Impl(AFHPlayerBase* Player, const UInputAction* InputAction, float HoldDuration)
{
	isInteractHold = true;
	if (!CartUIInstance)
	{
		CartUIInstance = CreateWidget<UInventoryBase>(Player->GetController<APlayerController>(), CartUIClass);
		CartUIInstance->AddToViewport(1);

		CartUIInstance->ActivateWidget();
		CartUIInstance->SetInventoryComponent(InventoryComponent);
	}
	else 
	{
		if (CartUIInstance->IsActivated())
		{
			CartUIInstance->DeactivateWidget();
		}
		else
		{
			CartUIInstance->ActivateWidget();
			CartUIInstance->SetInventoryComponent(InventoryComponent);
		}
	}
}

void AFHCart::OnInteractRelease_Impl(AFHPlayerBase* Player, const UInputAction* InputAction, float HoldDuration)
{
	if (!isInteractHold)
	{
		ToggleDrivePlayer(Player);
	}

	isInteract = false;
	isInteractHold = false;
}

void AFHCart::Move(const FInputActionValue& Value)
{
	const FVector2D MoveValue = Value.Get<FVector2D>();
	if (MoveValue.Y > 0)
	{
		GetChaosVehicleMovement()->SetThrottleInput(MoveValue.Y);
		GetChaosVehicleMovement()->SetBrakeInput(0);
	}
	else if (MoveValue.Y < 0)
	{
		GetChaosVehicleMovement()->SetThrottleInput(0);
		GetChaosVehicleMovement()->SetBrakeInput(-MoveValue.Y);
	}
	else
	{
		GetChaosVehicleMovement()->SetThrottleInput(0);
		GetChaosVehicleMovement()->SetBrakeInput(0);
	}
	GetChaosVehicleMovement()->SetSteeringInput(MoveValue.X);
}

TObjectPtr<UChaosWheeledVehicleMovementComponent> AFHCart::GetChaosVehicleMovement() const
{
	return CastChecked<UChaosWheeledVehicleMovementComponent>(GetVehicleMovement());
}

USiInventoryComponent* AFHCart::GetInventoryComponent() const
{
	return InventoryComponent;
}

void AFHCart::ToggleDrivePlayer(AFHPlayerBase* Player)
{
	if (GetInstigator<AFHPlayerBase>())
	{
		if (GetInstigator<AFHPlayerBase>() == Player)
		{
			AFHPlayerBase* OriginPlayer = GetInstigator<AFHPlayerBase>();
			OriginPlayer->GetRootComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			OriginPlayer->SetCart(nullptr);
			FRotator NewRotation = OriginPlayer->GetViewRotation();
			NewRotation.Pitch = 0.0f;
			NewRotation.Roll = 0.0f;
			OriginPlayer->SetActorRotation(NewRotation);
			OriginPlayer->GetController()->SetControlRotation(NewRotation);

			APlayerCameraManager* CameraManager = GetInstigator()->GetController<APlayerController>()->PlayerCameraManager;
			if (CameraManager)
			{
				CameraManager->ViewYawMin = 0.0f;
				CameraManager->ViewYawMax = 359.999f;
			}
			SetInstigator(nullptr);
		}
	}
	else
	{
		if (!IsFallOver())
		{
			SetInstigator(Player);
			Player->SetCart(this);
			Player->GetRootComponent()->AttachToComponent(CameraBoom, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		}
		else
		{
			//서서히 서게 보간 하기
			FRotator NewRotation = GetActorRotation();
			NewRotation.Pitch = 0.0f;
			NewRotation.Roll = 0.0f;
			SetActorRotation(FRotator::ZeroRotator, ETeleportType::TeleportPhysics);
		}
	}
}

bool AFHCart::IsFallOver() const
{
	return GetActorUpVector().Dot(FVector::DownVector) > -0.1f;
}

void AFHCart::SettingGameInstance()
{
	for (size_t i = 4; i < InventoryComponent->GetItemNum(); i++)
	{
		InventoryComponent->ItemLock(i, true);
	}
}

UCartVihecleWheelFront::UCartVihecleWheelFront()
{
	AxleType = EAxleType::Front;
	bAffectedBySteering = true;
	MaxSteerAngle = 40.f;

	WheelRadius = 30.0f;
	CorneringStiffness = 750.0f;
	FrictionForceMultiplier = 4.0f;
	bAffectedByEngine = true;

	SuspensionSmoothing = 10.0f;
	SuspensionMaxRaise = 30.0f;
	SuspensionMaxDrop = 30.0f;
	WheelLoadRatio = 1.0f;
	SpringRate = 100.0f;
	SpringPreload = 100.0f;
	SweepShape = ESweepShape::Spherecast;

	MaxBrakeTorque = 3000.0f;
	MaxHandBrakeTorque = 6000.0f;
}

UCartVihecleWheelRear::UCartVihecleWheelRear()
{
	AxleType = EAxleType::Rear;
	bAffectedByHandbrake = true;
	bAffectedByEngine = true;

	WheelRadius = 30.f;
	CorneringStiffness = 750.0f;
	FrictionForceMultiplier = 4.0f;

	SuspensionSmoothing = 10.0f;
	SuspensionMaxRaise = 30.0f;
	SuspensionMaxDrop = 30.0f;
	WheelLoadRatio = 1.0f;
	SpringRate = 100.0f;
	SpringPreload = 100.0f;
	SweepShape = ESweepShape::Spherecast;

	MaxBrakeTorque = 3000.0f;
	MaxHandBrakeTorque = 6000.0f;
}
