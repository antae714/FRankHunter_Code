// Copyright F Rank Hunter. All Rights Reserved.


#include "Props/FHReturnLobbyActor.h"
#include "Core/FHGateGameModeBase.h"
#include "Core/FHPlayerController.h"
#include "Player/FHPlayerBase.h"
#include "Component/FHInteractableComponent.h"

// Sets default values
AFHReturnLobbyActor::AFHReturnLobbyActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InteractableComp = CreateDefaultSubobject<UFHInteractableComponent>(TEXT("InteractableComp"));
}

// Called when the game starts or when spawned
void AFHReturnLobbyActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFHReturnLobbyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFHReturnLobbyActor::Interact_Impl(AFHPlayerBase* Player)
{
	AGameModeBase* GameMode = GetWorld()->GetAuthGameMode();
	if (GameMode)
	{
		AFHGateGameModeBase* GateGameMode = Cast<AFHGateGameModeBase>(GameMode);
		if (GateGameMode)
		{
			GateGameMode->BackToLobby();
		}
	}


}

