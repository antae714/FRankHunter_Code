// Copyright F Rank Hunter. All Rights Reserved.


#include "Props/FHDoubleDoorBase.h"
#include "FRankHunter.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Player/FHPlayerBase.h"
#include "Component/FHInteractableComponent.h"

AFHDoubleDoorBase::AFHDoubleDoorBase()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	/*DoorRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(DoorRoot);*/
	DoorFrameMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorFrameMesh"));
	SetRootComponent(DoorFrameMesh);

	LeftDoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftDoorMesh"));
	//LeftDoorMesh->SetSimulatePhysics(true);
	LeftDoorMesh->SetupAttachment(DoorFrameMesh);
	//LeftDoorMesh->SetMassOverrideInKg(NAME_None, 5000.0f, true);

	RightDoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightDoorMesh"));
	//RightDoorMesh->SetSimulatePhysics(true);
	RightDoorMesh->SetupAttachment(DoorFrameMesh);
	//RightDoorMesh->SetMassOverrideInKg(NAME_None, 5000.0f, true);

	LeftConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("LeftConstraint"));
	LeftConstraint->SetupAttachment(LeftDoorMesh);
	LeftConstraint->ComponentName1.ComponentName = TEXT("LeftDoorMesh");
	LeftConstraint->SetDisableCollision(true);
	LeftConstraint->SetLinearXLimit(ELinearConstraintMotion::LCM_Locked, 0);
	LeftConstraint->SetLinearYLimit(ELinearConstraintMotion::LCM_Locked, 0);
	LeftConstraint->SetLinearZLimit(ELinearConstraintMotion::LCM_Locked, 0);
	LeftConstraint->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Limited, RotationLimit);
	LeftConstraint->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Locked, 0);
	LeftConstraint->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Locked, 0);
	FConstraintInstance& LeftConstraintInst = LeftConstraint->ConstraintInstance;
	LeftConstraintInst.ProfileInstance.AngularDrive.SwingDrive.bEnablePositionDrive = true;
	LeftConstraintInst.ProfileInstance.AngularDrive.SwingDrive.bEnableVelocityDrive = true;
	LeftConstraintInst.SetAngularDriveMode(EAngularDriveMode::Type::TwistAndSwing);

	RightConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("RightConstraint"));
	RightConstraint->SetupAttachment(RightDoorMesh);
	RightConstraint->ComponentName1.ComponentName = TEXT("RightDoorMesh");
	RightConstraint->SetDisableCollision(true);
	RightConstraint->SetLinearXLimit(ELinearConstraintMotion::LCM_Locked, 0);
	RightConstraint->SetLinearYLimit(ELinearConstraintMotion::LCM_Locked, 0);
	RightConstraint->SetLinearZLimit(ELinearConstraintMotion::LCM_Locked, 0);
	RightConstraint->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Limited, RotationLimit);
	RightConstraint->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Locked, 0);
	RightConstraint->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Locked, 0);
	FConstraintInstance& RightConstraintInst = RightConstraint->ConstraintInstance;
	RightConstraintInst.ProfileInstance.AngularDrive.SwingDrive.bEnablePositionDrive = true;
	RightConstraintInst.ProfileInstance.AngularDrive.SwingDrive.bEnableVelocityDrive = true;
	RightConstraintInst.SetAngularDriveMode(EAngularDriveMode::Type::TwistAndSwing);

	InteractableComp = CreateDefaultSubobject<UFHInteractableComponent>(TEXT("InteractableComp"));

	DoorFrameMesh->IgnoreComponentWhenMoving(LeftDoorMesh, true);
	DoorFrameMesh->IgnoreComponentWhenMoving(RightDoorMesh, true);
	LeftDoorMesh->IgnoreComponentWhenMoving(DoorFrameMesh, true);
	RightDoorMesh->IgnoreComponentWhenMoving(DoorFrameMesh, true);
}

void AFHDoubleDoorBase::BeginPlay()
{
	if (HasAuthority())
	{
		InteractableComp->OnInteractServer.AddDynamic(this, &AFHDoubleDoorBase::OnInteract_Impl);
		InteractableComp->OnInteractHoldServer.AddDynamic(this, &AFHDoubleDoorBase::OnInteractHold_Impl);
		InteractableComp->OnInteractReleaseServer.AddDynamic(this, &AFHDoubleDoorBase::OnInteractRelease_Impl);
	}
}

void AFHDoubleDoorBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	LeftDoorMesh->SetSimulatePhysics(true);
	LeftDoorMesh->SetMassOverrideInKg(NAME_None, 5000.0f, true);
	RightDoorMesh->SetSimulatePhysics(true);
	RightDoorMesh->SetMassOverrideInKg(NAME_None, 5000.0f, true);
}

bool AFHDoubleDoorBase::IsDoorOpenDirectionForward(class AFHPlayerBase* Player)
{
	FVector PlayerPos = Player->GetActorLocation();
	FVector DoorPos = GetActorLocation();

	FVector DoorToPlayer = (PlayerPos - DoorPos).GetSafeNormal2D();

	FVector DoorRight = LeftDoorMesh->GetForwardVector();
	float dot = FVector::DotProduct(DoorToPlayer, DoorRight);

	if (bHingeIsRightOnFrontView)
	{
		return (dot > 0) ? true : false;
	}
	else
	{
		return (dot > 0) ? false : true;
	}
}

void AFHDoubleDoorBase::ToggleDoorOpen_Implementation(class AFHPlayerBase* Player, bool slowly)
{
	float movePower = slowly ? MotorStrengthWhenOpenSlowly : MotorStrengthWhenOpenForcefully;
	FRotator targetRot{ 0, 0, 0 };

	if (bIsOpened == true)
	{
		bIsOpened = false;
	}
	else
	{
		bIsOpened = true;

		targetRot.Yaw = IsDoorOpenDirectionForward(Player) ? RotationLimit : -RotationLimit;
	}

	HandleDoor(LeftDoorMesh, LeftConstraint, targetRot, movePower);
	targetRot.Yaw = -targetRot.Yaw;
	HandleDoor(RightDoorMesh, RightConstraint, targetRot, movePower);
}

void AFHDoubleDoorBase::OnInteract_Impl(class AFHPlayerBase* Player, const UInputAction* InputAction)
{
	bPushDoorSlowly = false;
}

void AFHDoubleDoorBase::OnInteractHold_Impl(class AFHPlayerBase* Player, const UInputAction* InputAction, float HoldDuration)
{
	ToggleDoorOpen(Player, true);
	bPushDoorSlowly = true;
}

void AFHDoubleDoorBase::OnInteractRelease_Impl(class AFHPlayerBase* Player, const UInputAction* InputAction, float HoldDuration)
{
	if (bPushDoorSlowly == false)
	{
		ToggleDoorOpen(Player, false);
	}
}

void AFHDoubleDoorBase::HandleDoor(class UStaticMeshComponent* _DoorMesh, class UPhysicsConstraintComponent* _PhysicsConstraint, FRotator AngularOrientation, float movePower)
{
	FConstraintInstance& ConstraintInst = _PhysicsConstraint->ConstraintInstance;
	float inSpring{};
	float damping{};
	float forceLimit{};

	_PhysicsConstraint->SetAngularOrientationTarget(AngularOrientation);
	ConstraintInst.GetAngularDriveParams(inSpring, damping, forceLimit);
	ConstraintInst.SetAngularDriveParams(movePower, damping, forceLimit);

	_DoorMesh->WakeRigidBody();
}
