// Copyright F Rank Hunter. All Rights Reserved.


#include "Props/FHMagicExtractor.h"

#include "Component/FHInteractableComponent.h"

#include "FHDisplayBoard.h"
#include "Lobby/FH_GS_LobbyGameState.h"
#include "Core/FHPlayerController.h"
#include "Player/FHPlayerBase.h"

// Sets default values
AFHMagicExtractor::AFHMagicExtractor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	InteractableComp = CreateDefaultSubobject<UFHInteractableComponent>(TEXT("InteractableComp"));

}

// Called when the game starts or when spawned
void AFHMagicExtractor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFHMagicExtractor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFHMagicExtractor::Interact_Impl(class AFHPlayerBase* Player)
{
	AFHPlayerController* PC = Cast<AFHPlayerController>(Player->GetController());
	if (PC)
	{
		PC->OpenPaymentUI();
	}
}

