// Copyright F Rank Hunter. All Rights Reserved.


#include "Props/FHDoorBase.h"
#include "FRankHunter.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Player/FHPlayerBase.h"
#include "Component/FHInteractableComponent.h"

#include "BluePrintFunctions/FHBlueprintFunctionLibrary.h"
#include "Core/FHSoundManagerActor.h"

AFHDoorBase::AFHDoorBase()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	DoorRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DoorRoot"));
	SetRootComponent(DoorRoot);

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	//DoorMesh->SetSimulatePhysics(true);
	DoorMesh->SetupAttachment(DoorRoot);
	// 안 밀리게
	//DoorMesh->SetMassOverrideInKg(NAME_None, 5000.0f, true);

	PhysicsConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("PhysicsConstraint"));
	PhysicsConstraint->SetupAttachment(DoorMesh);

	PhysicsConstraint->ComponentName1.ComponentName = TEXT("DoorMesh");
	PhysicsConstraint->SetDisableCollision(true);
	PhysicsConstraint->SetLinearXLimit(ELinearConstraintMotion::LCM_Locked, 0);
	PhysicsConstraint->SetLinearYLimit(ELinearConstraintMotion::LCM_Locked, 0);
	PhysicsConstraint->SetLinearZLimit(ELinearConstraintMotion::LCM_Locked, 0);

	PhysicsConstraint->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Limited, RotationLimit);
	//FRotator newAngularRotationOffset{ 0, -RotationLimit, 0 };
	//PhysicsConstraint->ConstraintInstance.AngularRotationOffset = newAngularRotationOffset;
	//PhysicsConstraint->SetAngularOrientationTarget(FRotator(0, 0, 0));

	PhysicsConstraint->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Locked, 0);
	PhysicsConstraint->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Locked, 0);

	FConstraintInstance& ConstraintInst = PhysicsConstraint->ConstraintInstance;
	ConstraintInst.ProfileInstance.AngularDrive.SwingDrive.bEnablePositionDrive = true;
	ConstraintInst.ProfileInstance.AngularDrive.SwingDrive.bEnableVelocityDrive = true;
	ConstraintInst.SetAngularDriveMode(EAngularDriveMode::Type::TwistAndSwing);


	InteractableComp = CreateDefaultSubobject<UFHInteractableComponent>(TEXT("InteractableComp"));
}

void AFHDoorBase::BeginPlay()
{
	Super::BeginPlay();

	initialYaw = GetActorRotation().Yaw;

	if (HasAuthority())
	{
		InteractableComp->OnInteractServer.AddDynamic(this, &AFHDoorBase::OnInteract_Impl);
		InteractableComp->OnInteractHoldServer.AddDynamic(this, &AFHDoorBase::OnInteractHold_Impl);
		InteractableComp->OnInteractReleaseServer.AddDynamic(this, &AFHDoorBase::OnInteractRelease_Impl);
	}
}

void AFHDoorBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();


	DoorMesh->SetSimulatePhysics(true);
	DoorMesh->SetMassOverrideInKg(NAME_None, 5000.0f, true);
}

void AFHDoorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFHDoorBase::OpenDoorForcefully(AActor* Subject, float ForceMult)
{
	if (bIsOpened == false)
	{
		ToggleDoorOpen(Subject, MotorStrengthWhenOpenForcefully * ForceMult);
	}
}

//float AFHDoorBase::GetReverseTorqueDirection(class AFHPlayerBase* Player)
//{
//	FVector pPos = Player->GetActorLocation();
//	FVector dPos = GetActorLocation();
//	
//	FVector dToP = (pPos - dPos).GetSafeNormal2D();
//
//	FVector dRight = DoorMesh->GetForwardVector();
//	float dot = FVector::DotProduct(dToP, dRight);
//	
//	if (bHingeIsRightOnFrontView == false)
//	{
//		return (dot > 0) ? -1.0f : 1.0f;
//	}
//	else
//	{
//		return (dot > 0) ? 1.0f : -1.0f;
//	}
//}

//bool AFHDoorBase::IsDoorClosed()
//{
//	float deltaYaw = GetDeltaAngleFromInitialDegree();
//	if (FMath::Abs(deltaYaw) <= ClosedAngleThreshold)
//	{
//		return true;
//	}
//	return false;
//}

//float AFHDoorBase::GetDeltaAngleFromInitialDegree()
//{
//	float currentYaw = GetActorRotation().Yaw;
//	float deltaYaw = FMath::FindDeltaAngleDegrees(currentYaw, initialYaw);
//
//	return deltaYaw;
//}

bool AFHDoorBase::IsDoorOpenDirectionForward(AActor* Subject)
{
	FVector SubjectPos = Subject->GetActorLocation();
	FVector DoorPos = GetActorLocation();
	
	FVector DoorToPlayer = (SubjectPos - DoorPos).GetSafeNormal2D();

	FVector DoorRight = DoorMesh->GetForwardVector();
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

void AFHDoorBase::ToggleDoorOpen_Implementation(AActor* Subject, float power, bool alsoHandlePair)
{
	/*float deltaYaw = GetDeltaAngleFromInitialDegree();

	if (IsDoorClosed())
	{
		float torqueDir = GetReverseTorqueDirection(Player);
		FVector torque = FVector(0, 0, torqueDir * TorquePerDegree * (90.0f - FMath::Abs(deltaYaw)));
		DoorMesh->AddAngularImpulseInDegrees(torque, NAME_None, true);
	}
	else
	{
		FVector torque = FVector(0, 0, deltaYaw * TorquePerDegree);
		DoorMesh->AddAngularImpulseInDegrees(torque, NAME_None, true);
	}*/

	FRotator targetRot{ 0, 0, 0 };



	// ======== Play Sound =========
	AFHSoundManagerActor* SoundManager = UFHBlueprintFunctionLibrary::GetSoundManager(GetWorld());
	if (SoundManager)
	{
		bool IsNoiseReport = Subject->IsA<AFHPlayerBase>();
		if (bIsOpened == true)
		{
			SoundManager->PlaySoundAtLocationLocallyByTag(GET_GAMEPLAY_TAG("Sound.Interact.TempleDoor.Close"), GetActorLocation(), 1.0f, IsNoiseReport, 3.0f, this);
		}
		else
		{
			SoundManager->PlaySoundAtLocationLocallyByTag(GET_GAMEPLAY_TAG("Sound.Interact.TempleDoor.Open"), GetActorLocation(), 1.0f, IsNoiseReport, 3.0f, this);
		}
	}



	if (bIsOpened == true)
	{
		bIsOpened = false;
	}
	else
	{
		bIsOpened = true;

		targetRot.Yaw = IsDoorOpenDirectionForward(Subject) ? OpenDegree : -OpenDegree;
	}

	HandleDoor(DoorMesh, PhysicsConstraint, targetRot, power);
	if (alsoHandlePair && PairDoor != nullptr)
	{
		PairDoor->bIsOpened = bIsOpened;
		targetRot.Yaw = -targetRot.Yaw;
		HandleDoor(PairDoor->GetStaticMesh(), PairDoor->GetPhysicsConstraint(), targetRot, power);
	}
}

void AFHDoorBase::OnInteract_Impl(AFHPlayerBase* Player, const UInputAction* InputAction)
{
	ToggleDoorOpen(Player, MotorStrengthWhenOpenForcefully);
	//bPushDoorSlowly = false;
}

void AFHDoorBase::OnInteractHold_Impl(AFHPlayerBase* Player, const UInputAction* InputAction, float HoldDuration)
{
	//ToggleDoorOpen(Player, MotorStrengthWhenOpenSlowly);
	//bPushDoorSlowly = true;
}

void AFHDoorBase::OnInteractRelease_Impl(AFHPlayerBase* Player, const UInputAction* InputAction, float HoldDuration)
{
	/*if (bPushDoorSlowly == false)
	{
		ToggleDoorOpen(Player, MotorStrengthWhenOpenForcefully);
	}*/
}

void AFHDoorBase::HandleDoor(UStaticMeshComponent* _DoorMesh, UPhysicsConstraintComponent* _PhysicsConstraint, FRotator AngularOrientation, float movePower)
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
