// Copyright F Rank Hunter. All Rights Reserved.


#include "Props/FHDisplayBoard.h"

#include "Components/WidgetComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Lobby/FHLobbyGameMode.h"
#include "Lobby/FH_GS_LobbyGameState.h"
#include "Kismet/GameplayStatics.h"

#include "Core/FHPropManager.h"
#include "Core/ManagerActorRegistrySubsystem.h"

#include "Lobby/FH_GS_LobbyGameState.h"



// Sets default values
AFHDisplayBoard::AFHDisplayBoard()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	RootComponent = Body;

	Widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Screen"));
	Widget->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AFHDisplayBoard::BeginPlay()
{
	Super::BeginPlay();

	UManagerActorRegistrySubsystem* Subsystem = GetWorld()->GetSubsystem<UManagerActorRegistrySubsystem>();
	Subsystem->GetManagerRegistDelegate(TEXT("PropManager")).AddDynamic(this, &AFHDisplayBoard::NotifyPropManagerRegisted);

	AFH_GS_LobbyGameState* GameState = Cast<AFH_GS_LobbyGameState>(UGameplayStatics::GetGameState(GetWorld()));
	if (GameState)
	{
		GameState->OnChangedRequiredMoney.AddDynamic(this, &ThisClass::SetCurrentQuota);
		GameState->OnChangedPlayerMoney.AddDynamic(this, &ThisClass::AddExtractResult);
		SetCurrentQuota(GameState->GetRequiredMoney());
		AddExtractResult(GameState->GetMoney());
	}

}

// Called every frame
void AFHDisplayBoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFHDisplayBoard::NotifyPropManagerRegisted()
{
	UManagerActorRegistrySubsystem* Subsystem = GetWorld()->GetSubsystem<UManagerActorRegistrySubsystem>();
	AFHPropManager* PropManager = Subsystem->GetManagerActor<AFHPropManager>(TEXT("PropManager"));
	PropManager->RegistProp(TEXT("DisplayBoard"), this);
}

