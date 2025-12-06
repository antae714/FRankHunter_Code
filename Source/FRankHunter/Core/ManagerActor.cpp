// Copyright F Rank Hunter. All Rights Reserved.


#include "Core/ManagerActor.h"
#include "Core/ManagerActorRegistrySubsystem.h"

// Sets default values
AManagerActor::AManagerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

// Called when the game starts or when spawned
void AManagerActor::BeginPlay()
{
	Super::BeginPlay();
	
	UManagerActorRegistrySubsystem* ManagerActorRegistrySubsystem = GetWorld()->GetSubsystem<UManagerActorRegistrySubsystem>();
	if (ManagerActorRegistrySubsystem)
	{
		if (!ManagerActorRegistrySubsystem->RegisterManagerActor(this))
		{
			Destroyed();
		}
	}
}

void AManagerActor::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UManagerActorRegistrySubsystem* ManagerActorRegistrySubsystem = GetWorld()->GetSubsystem<UManagerActorRegistrySubsystem>();
	if (ManagerActorRegistrySubsystem)
	{
		check(ManagerActorRegistrySubsystem->UnRegisterManagerActor(this));
	}
}
