// Copyright F Rank Hunter. All Rights Reserved.


#include "Props/Traps/FHArrowTrap.h"
#include "Net/UnrealNetwork.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "BluePrintFunctions/FHBlueprintFunctionLibrary.h"
#include "Core/FHSoundManagerActor.h"
#include "GAS/FHGameplayTags.h"

AFHArrowTrap::AFHArrowTrap()
{
}

void AFHArrowTrap::BeginPlay()
{
	Super::BeginPlay();
}

void AFHArrowTrap::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AFHArrowTrap::Server_ShotArrow_Implementation()
{
	TArray<UArrowComponent*> ArrowSpawnComponents;
	GetComponents<UArrowComponent>(ArrowSpawnComponents);

	for (UArrowComponent* ArrowComp : ArrowSpawnComponents)
	{
		FVector SpawnPos = ArrowComp->GetComponentLocation();
		FRotator SpawnRot = ArrowComp->GetComponentRotation() + ArrowRotation;

		FActorSpawnParameters SpawnParam;
		AActor* Arrow = GetWorld()->SpawnActor<AActor>(ArrowActorClass, SpawnPos, SpawnRot);
	}

	// ======== Play Sound =========
	AFHSoundManagerActor* SoundManager = UFHBlueprintFunctionLibrary::GetSoundManager(GetWorld());
	if (SoundManager)
	{
		SoundManager->PlaySoundAtLocationMulticastByTag(GET_GAMEPLAY_TAG("Sound.Trap.Active.Xbow"), GetActorLocation());
	}
}