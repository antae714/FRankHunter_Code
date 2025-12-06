// Copyright F Rank Hunter. All Rights Reserved.


#include "Component/ActorRPC/FHActorRPCComponent.h"
#include "Core/FHPlayerStateBase.h"
#include "Core/FHGateGameModeBase.h"
#include "Lobby/FHLobbyGameMode.h"
#include "Kismet/GameplayStatics.h"

void UFHActorRPCComponent::SetSkill_Implementation(FName SkillID)
{
	APlayerController* Controller = GetOwner<APlayerController>();
	if (!Controller)
	{
		return;
	}

	AFHPlayerStateBase* PlayerState = Controller->GetPlayerState<AFHPlayerStateBase>();
	if (!PlayerState)
	{
		return;
	}

	PlayerState->SetSkillID(SkillID);
}

void UFHActorRPCComponent::EnterReady_Implementation()
{
	AFHGateGameModeBase* GateGM = Cast<AFHGateGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	AFHLobbyGameMode* LobbyGM = Cast<AFHLobbyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (LobbyGM)
	{
		LobbyGM->PlayerEnterReady();
	}
	if (GateGM)
	{
		GateGM->PlayerEnterReady();
	}
}
