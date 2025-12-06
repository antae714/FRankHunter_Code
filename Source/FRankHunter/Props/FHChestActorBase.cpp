// Copyright F Rank Hunter. All Rights Reserved.

#include "Props/FHChestActorBase.h"
#include "FRankHunter.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Player/FHPlayerBase.h"
#include "Item/Actors/FHPickupItemActor.h"
#include "Component/FHInteractableComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DataAsset/FHChestDescriptor.h"
#include "Algo/RandomShuffle.h"
#include "Containers/Array.h"
#include "Item/FHInventoryComponent.h"
#include "Item/FHItemBase.h"
#include "Core/FHPlayerStateBase.h"
#include "UI/Tooltip/FHInteractTooltipWidget.h"
#include "Net/UnrealNetwork.h"
#include "Item/FHItemBase.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Core/FHGateGameModeBase.h"
#include "DungeonGeneration/FHDungeonGenerator.h"

#include "BluePrintFunctions/FHBlueprintFunctionLibrary.h"
#include "Core/FHSoundManagerActor.h"


AFHChestActorBase::AFHChestActorBase()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	ChestBodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChestBodyMesh"));
	SetRootComponent(ChestBodyMesh);
	ChestLidMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChestLidMesh"));
	ChestLidMesh->SetupAttachment(ChestBodyMesh);
	//ChestLidMesh->SetSimulatePhysics(true);

	PhysicsConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("PhysicsConstraint"));
	PhysicsConstraint->SetupAttachment(ChestLidMesh);


	PhysicsConstraint->ComponentName1.ComponentName = TEXT("ChestLidMesh");
	PhysicsConstraint->ComponentName2.ComponentName = TEXT("ChestBodyMesh");
	PhysicsConstraint->SetDisableCollision(true);
	PhysicsConstraint->SetLinearXLimit(ELinearConstraintMotion::LCM_Locked, 0);
	PhysicsConstraint->SetLinearYLimit(ELinearConstraintMotion::LCM_Locked, 0);
	PhysicsConstraint->SetLinearZLimit(ELinearConstraintMotion::LCM_Locked, 0);
	PhysicsConstraint->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Locked, 0);
	PhysicsConstraint->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Locked, 0);

	PhysicsConstraint->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Limited, RotationLimit);
	FRotator newAngularRotationOffset{ RotationLimit, 0, 0 };
	PhysicsConstraint->ConstraintInstance.AngularRotationOffset = newAngularRotationOffset;
	// À­¶Ñ²±ÀÌ ¹«°Å¿ì¸é -0.5·Î ÀâÀ¸¸é »ìÂ¦ ´Þ¶û°Å¸®´Â ´À³¦ÀÌ ³­´Ù;;
	PhysicsConstraint->SetAngularOrientationTarget(FRotator(RotationLimit * -0.7f, 0, 0));


	FConstraintInstance& ConstraintInst = PhysicsConstraint->ConstraintInstance;
	ConstraintInst.ProfileInstance.AngularDrive.SwingDrive.bEnablePositionDrive = true;
	ConstraintInst.ProfileInstance.AngularDrive.SwingDrive.bEnableVelocityDrive = true;
	ConstraintInst.SetAngularDriveMode(EAngularDriveMode::Type::TwistAndSwing);

	//ForceAnchorSocketName = TEXT("ForceAnchor");

	InteractableComp = CreateDefaultSubobject<UFHInteractableComponent>(TEXT("InteractableComp"));
	InteractableComp->OnInteractServer.AddDynamic(this, &AFHChestActorBase::OnInteract_Impl);
	InteractableComp->OnInteractHoldServer.AddDynamic(this, &AFHChestActorBase::OnInteractHold_Impl);
	InteractableComp->OnInteractReleaseServer.AddDynamic(this, &AFHChestActorBase::OnInteractRelease_Impl);
}

void AFHChestActorBase::BeginPlay()
{
	Super::BeginPlay();

	InteractableComp->SetFirstTooltipText(DefaultInteractTooltip);

	if (ChestDescriptor == nullptr)
	{
		return;
	}

	GetWorld()->GetTimerManager().SetTimerForNextTick(
		FTimerDelegate::CreateLambda([this]()
	{
		if (HasAuthority())
		{
			if (ChestDescriptor->bCanSpawnedAsChestTrap == true)
			{
				float trapChance = FMath::RandRange(0.0f, 1.0f);
				bIsTrap = trapChance <= ChestDescriptor->SpawnedAsChestTrapCance ? true : false;

				if (bIsTrap) return;
			}

			float lockChance = FMath::RandRange(0.0f, 1.0f);
			bIsLocked = lockChance <= ChestDescriptor->LockChance ? true : false;

			OnRep_bIsLocked();

			for (int32 i = 1; i <= SpawnPointSocketNum; ++i)
			{
				RandomlyAlignedSocketNames.Add(FString::Printf(TEXT("%s%d"), *SpawnPointSocketNameBase, i));
			}
			Algo::RandomShuffle(RandomlyAlignedSocketNames);

			SpawnItem();
		}
	})
	);
}

void AFHChestActorBase::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	Super::EndPlay(EndPlayReason);

	if (TrapTimer.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(TrapTimer);
	}
}

void AFHChestActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AFHChestActorBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	ChestLidMesh->SetSimulatePhysics(true);
}

void AFHChestActorBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFHChestActorBase, bIsLocked);
	DOREPLIFETIME(AFHChestActorBase, bIsTrap);
}

void AFHChestActorBase::OnRep_bIsLocked()
{
	if (bIsLocked)
	{
		ChestLidMesh->SetSimulatePhysics(false);
	}
	else
	{
	}
}

void AFHChestActorBase::OnRep_bIsTrap()
{

}

void AFHChestActorBase::SpawnItem()
{
	if (!ChestDescriptor)
	{
		PRINT_LOG(TEXT("There is no chest descriptor... Faild to spawn item."));
		return;
	}
	if (ChestDescriptor->SpawnableList.IsEmpty())
	{
		return;
	}

	int32 SpawnNum;
	if (ChestDescriptor->bUseRandomNum)
	{
		SpawnNum = FMath::RandRange(ChestDescriptor->MinSpawnItemNum, ChestDescriptor->MaxSpawnItemNum);
	}
	else
	{
		SpawnNum = ChestDescriptor->SpawnItemNum;
	}

	if (RandomlyAlignedSocketNames.Num() < SpawnNum)
	{
		SpawnNum = RandomlyAlignedSocketNames.Num();
	}


	TArray<float> weights;
	for (int i = 0; i < ChestDescriptor->SpawnableList.Num(); ++i)
	{
		weights.Add(ChestDescriptor->SpawnableList[i].Weight);
	}
	for (int i = 0; i < SpawnNum; ++i)
	{
		int32 targetIdx = UFHBlueprintFunctionLibrary::GetRandIndexUsingDiscreteDistribution(weights);
		FString socketName = RandomlyAlignedSocketNames[i];
		FTransform socketTransform = ChestBodyMesh->GetSocketTransform(*socketName);

		if (ChestDescriptor->bRandomRotationY)
		{
			FRotator rotation = socketTransform.GetRotation().Rotator();
			rotation.Yaw = FMath::RandRange(0.0, 360.0);
			socketTransform.SetRotation(rotation.Quaternion());
		}

		Blueprint_SpawnItem(socketTransform, ChestDescriptor->SpawnableList[targetIdx].Item);

		/*AActor* spawnedItem = GetWorld()->SpawnActor<AActor>(ChestDescriptor->SpawnableList[targetIdx].Actor, socketTransform);

		UFHInteractableComponent* interactComp = spawnedItem->GetComponentByClass<UFHInteractableComponent>();
		interactComp->bIsHighlightable = false;*/

		/*UFHInteractableComponent* interactable = spawnedItem->GetComponentByClass<UFHInteractableComponent>();
		if (interactable)
		{
			interactable->bIsHighlightable = false;
		}*/
	}
	

	/*GetWorld()->GetTimerManager().SetTimer(timerHandle,
		[this]()
	{
		FString socketName = ItemSpawnSocketName + FString::FromInt(FMath::RandRange(1, 3));
		FTransform socketTransform = ChestBodyMesh->GetSocketTransform(*socketName);
		AFHPickupItemActor* item = GetWorld()->SpawnActor<AFHPickupItemActor>(TestItemActor, socketTransform);
		float rrrrr = FMath::RandRange(0.05f, 0.3f);
		item->SetActorScale3D(FVector(rrrrr, rrrrr, rrrrr));
		UStaticMeshComponent* meshComp = item->GetComponentByClass<UStaticMeshComponent>();
		meshComp->AddImpulse(FVector(FMath::RandRange(-1.0f, 1.0f), 0, 1) * FMath::RandRange(8000, 48000) * rrrrr * rrrrr * rrrrr);
	}, 0.2f, true);*/
}

void AFHChestActorBase::ToggleChestOpen_Implementation()
{
	// ======== Play Sound =========
	AFHSoundManagerActor* SoundManager = UFHBlueprintFunctionLibrary::GetSoundManager(GetWorld());
	if (SoundManager)
	{
		if (bIsOpened == true)
		{
			SoundManager->PlaySoundAtLocationLocallyByTag(GET_GAMEPLAY_TAG("Sound.Interact.Chest.Close"), GetActorLocation(), 1.0f, true, 3.0f, this);
		}
		else
		{
			SoundManager->PlaySoundAtLocationLocallyByTag(GET_GAMEPLAY_TAG("Sound.Interact.Chest.Open"), GetActorLocation(), 1.0f, true, 3.0f, this);
		}
	}

	FConstraintInstance& ConstraintInst = PhysicsConstraint->ConstraintInstance;
	if (bIsOpened)
	{
		bIsOpened = false;

		// À­¶Ñ²±ÀÌ ¹«°Å¿ì¸é -0.5·Î ÀâÀ¸¸é »ìÂ¦ ´Þ¶û°Å¸®´Â ´À³¦ÀÌ ³­´Ù;;
		PhysicsConstraint->SetAngularOrientationTarget(FRotator(RotationLimit * -0.7f, 0, 0));
		ChestLidMesh->WakeRigidBody();
	}
	else
	{
		if (HasAuthority() && bIsFirstOpen)
		{
			bIsFirstOpen = false;
			//SpawnItem();
		}

		bIsOpened = true;
		ChestLidMesh->SetSimulatePhysics(true);

		PhysicsConstraint->SetAngularOrientationTarget(FRotator(RotationLimit * 0.5f, 0, 0));
		
		float inSpring{};
		float damping{};
		float forceLimit{};
		ConstraintInst.GetAngularDriveParams(inSpring, damping, forceLimit);
		ConstraintInst.SetAngularDriveParams(OpenLidMotorStrength, damping, forceLimit);


		ChestLidMesh->WakeRigidBody();
	}



	// ÁÝ±â°¡ Èûµé¾î¼­ ÀÏ´Ü..
	if (bIsOpened)
	{
		InteractableComp->bIsHighlightable = false;
		InteractableComp->bIgnoreThisActorFromInteractCheck = true;

		// SetActive¸¦ false·Î ¹Ù²Ù¸é ComponentTickÀÌ ¾È µ·´Ù
		//InteractableComp->SetActive(false);
	}
}

void AFHChestActorBase::UnlockChest_Implementation(bool bResult)
{
	if (bResult)
	{
		bIsLocked = false;

		PhysicsConstraint->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Limited, RotationLimit);

		AFHSoundManagerActor* SoundManager = UFHBlueprintFunctionLibrary::GetSoundManager(GetWorld());
		SoundManager->PlaySoundAtLocationLocallyByTag(GET_GAMEPLAY_TAG("Sound.Item.Use.Key"), GetActorLocation(), 1.0f, true, 3.0f, this);
	}
	else
	{
		InteractableComp->SetFirstTooltipText(LockedInteractTooltip);

		AFHSoundManagerActor* SoundManager = UFHBlueprintFunctionLibrary::GetSoundManager(GetWorld());
		if (SoundManager)
		{
			SoundManager->PlaySoundAtLocationLocallyByTag(GET_GAMEPLAY_TAG("Sound.Interact.Chest.LockedTry"), GetActorLocation(), 1.0f, false);
		}
	}
}

void AFHChestActorBase::OnInteract_Impl(AFHPlayerBase* Player, const UInputAction* InputAction)
{
	if (bIsLocked)
	{
		UFHInventoryComponent* PlayerInventory = Player->GetPlayerState()->GetComponentByClass<UFHInventoryComponent>();
		// if assert this, check player inventory component is exist.
		check(PlayerInventory);

		int32 CurrentItemIndex = PlayerInventory->GetCurrentItemIndex();
		UFHItemBase* ItemInstance = PlayerInventory->GetItemInstance<UFHItemBase>(CurrentItemIndex);
		if (!ItemInstance)
		{
			UnlockChest(false);
			return;
		}
		FFHItemData& ItemData = ItemInstance->GetItemData();
		if (!(ItemData.ItemID == TEXT("Key")))
		{
			UnlockChest(false);
			return;
		}
		PlayerInventory->Server_RemoveItemAtIndex(CurrentItemIndex, 1);

		UnlockChest(true);
		ToggleChestOpen();

		if (bIsTrap)
		{
			ActivateTeleportTrap(Player);
		}
	}
	else
	{
		ToggleChestOpen();

		if (bIsTrap)
		{
			ActivateTeleportTrap(Player);
		}
	}
}

void AFHChestActorBase::OnInteractHold_Impl(AFHPlayerBase* Player, const UInputAction* InputAction, float HoldDuration)
{

}

void AFHChestActorBase::OnInteractRelease_Impl(AFHPlayerBase* Player, const UInputAction* InputAction, float HoldDuration)
{

}

void AFHChestActorBase::ActivateTeleportTrap(class AFHPlayerBase* Target)
{
	FVector SpawnLocation = Target->GetActorLocation();

	FFindFloorResult FloorResult;
	Target->GetCharacterMovement()->FindFloor(Target->GetActorLocation(), FloorResult, true);
	if (FloorResult.bBlockingHit)
	{
		SpawnLocation.Z = FloorResult.HitResult.ImpactPoint.Z;
	}

	TeleportLocation = Target->GetActorLocation();
	AFHGateGameModeBase* GameMode = Cast<AFHGateGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{
		if (GameMode->GetGateGeneratorInstance())
		{
			TeleportLocation = GameMode->GetGateGeneratorInstance()->GetRandomRoomLocation(Target->GetActorLocation(), 300.f);

			//float BottomOffset = Target->GetComponentByClass<UCapsuleComponent>()->GetScaledCapsuleHalfHeight();
			//Target->SetActorLocation(RandomLocation, false, nullptr);
		}
	}

	SpawnFogNiagara(Target->GetActorLocation(), TeleportLocation);
	GetWorldTimerManager().SetTimer(TrapTimer,
		FTimerDelegate::CreateLambda([this, Target]()
	{
		bool success = Target->TeleportTo(TeleportLocation, Target->GetActorRotation());
	}),
	0.75f, false);
}

void AFHChestActorBase::SpawnFogNiagara_Implementation(FVector Loc1, FVector Loc2)
{
	if (FogNiagara)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			FogNiagara,
			Loc1,
			FRotator::ZeroRotator,
			FVector::One(),
			true
		);

		if (Loc1 != Loc2)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				FogNiagara,
				Loc2,
				FRotator::ZeroRotator,
				FVector::One(),
				true
			);
		}
	}
}

