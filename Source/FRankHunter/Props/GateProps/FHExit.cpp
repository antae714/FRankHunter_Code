// Copyright F Rank Hunter. All Rights Reserved.


#include "Props/GateProps/FHExit.h"
#include "Kismet/GameplayStatics.h"
#include "Core/FHGateGameStateBase.h"

// Sets default values
AFHExit::AFHExit()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

// Called when the game starts or when spawned
void AFHExit::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFHExit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFHExit::ShowResult()
{
	AFHGateGameStateBase* GameState = Cast<AFHGateGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));
	if (GameState)
	{
		GameState->ShowExplorationResult();
	}
}

