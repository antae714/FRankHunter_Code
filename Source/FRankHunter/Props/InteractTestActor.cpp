// Copyright F Rank Hunter. All Rights Reserved.

#include "Props/InteractTestActor.h"
#include "Player/FHPlayerBase.h"
#include "GameFramework/GameModeBase.h"
#include "Lobby/FHLobbyGameMode.h"
#include "Component/FHInteractableComponent.h"

// Sets default values
AInteractTestActor::AInteractTestActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	InteractableComp = CreateDefaultSubobject<UFHInteractableComponent>(TEXT("InteractableComp"));
}

// Called when the game starts or when spawned
void AInteractTestActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInteractTestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInteractTestActor::Interact_Impl(AFHPlayerBase* Player)
{
	UE_LOG(LogTemp, Warning, TEXT("%s Interacts with %s"), *Player->GetName(), *GetName());
	AGameModeBase* GameMode = GetWorld()->GetAuthGameMode();
	if (GameMode)
	{
		AFHLobbyGameMode* FHLobbyGameMode = Cast<AFHLobbyGameMode>(GameMode);
		if (FHLobbyGameMode)
		{
			FHLobbyGameMode->ChangeMap();
		}
	}
}


