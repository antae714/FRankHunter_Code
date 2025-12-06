// Copyright F Rank Hunter. All Rights Reserved.


#include "Props/Traps/FHChestTrap.h"
#include "Component/FHInteractableComponent.h"
#include "Player/FHPlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FRankHunter.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/CapsuleComponent.h"

#include "Core/FHGateGameModeBase.h"
#include "DungeonGeneration/FHDungeonGenerator.h"


AFHChestTrap::AFHChestTrap()
{
	ChestMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChestMesh"));
	SetRootComponent(ChestMesh);

	InteractableComponent = CreateDefaultSubobject<UFHInteractableComponent>(TEXT("InteractableComponent"));
}


void AFHChestTrap::BeginPlay()
{
	Super::BeginPlay();

	//FogActor = GetWorld()->SpawnActor<AActor>(FogActorClass);
}

void AFHChestTrap::Interact_Impl(AFHPlayerBase* Player)
{
	InteractableComponent->SetIsInteractEnable(true);

	CachedPlayer = Player;
	FVector SpawnLocation = Player->GetActorLocation();

	FFindFloorResult FloorResult;
	Player->GetCharacterMovement()->FindFloor(Player->GetActorLocation(), FloorResult, true);
	if (FloorResult.bBlockingHit)
	{
		SpawnLocation.Z = FloorResult.HitResult.ImpactPoint.Z;
	}
	//FogActor->SetActorLocation(SpawnLocation);

	UNiagaraComponent* Niagara = UNiagaraFunctionLibrary::SpawnSystemAttached(
		NiagaraSystem,
		Player->GetRootComponent(),
		NAME_None,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::KeepRelativeOffset,
		true
	);
}

void AFHChestTrap::TPPlayer()
{
	AFHGateGameModeBase* GameMode = Cast<AFHGateGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{
		if (GameMode->GetGateGeneratorInstance())
		{
			PRINT_LOG(TEXT("Player Location Before Interaction. {%f, %f, %f}"), 
				CachedPlayer->GetActorLocation().X, CachedPlayer->GetActorLocation().Y, CachedPlayer->GetActorLocation().Z)

			FVector RandomLocation = GameMode->GetGateGeneratorInstance()->GetRandomRoomLocation(CachedPlayer->GetActorLocation(), 300.f);
			PRINT_LOG(TEXT("Random TP Location. {%f, %f, %f}"), RandomLocation.X, RandomLocation.Y, RandomLocation.Z);

			float BottomOffset = CachedPlayer->GetComponentByClass<UCapsuleComponent>()->GetScaledCapsuleHalfHeight();
			RandomLocation.Z += BottomOffset;

			PRINT_LOG(TEXT("Capsule Half Height: %f"), BottomOffset);
			PRINT_LOG(TEXT("Changed TP Location. {%f, %f, %f}"), RandomLocation.X, RandomLocation.Y, RandomLocation.Z);

			CachedPlayer->SetActorLocation(RandomLocation, false, nullptr);
		}
	}
	
	if (TPPositionActor)
	{
		FVector TPLocation = TPPositionActor->GetActorLocation();
		CachedPlayer->SetActorLocation(TPLocation, false, nullptr);
		//FogActor->SetActorLocation(TPLocation, false, nullptr);
	}
	else
	{
		PRINT_LOG(TEXT("TPPosition Actor Not Setted."));
	}
}
