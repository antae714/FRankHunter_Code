// Copyright F Rank Hunter. All Rights Reserved.


#include "Component/WidgetRPC/FHReturnCoreRPCComponent.h"
#include "Lobby/FHLobbyGameMode.h"
#include "Kismet/GameplayStatics.h"

void UFHReturnCoreRPCComponent::ReturnCore_Implementation()
{
	AFHLobbyGameMode* GameMode = Cast<AFHLobbyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GameMode)
	{
		return;
	}

	APlayerController* Owner = GetOwner<APlayerController>();
	if (!Owner)
	{
		return;
	}

	GameMode->ReturnCore(Owner);
}
