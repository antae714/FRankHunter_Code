// Copyright F Rank Hunter. All Rights Reserved.


#include "UI/Lobby/FHRestRequest.h"
#include "Kismet/GameplayStatics.h"
#include "Lobby/FH_GS_LobbyGameState.h"
#include "Component/WidgetRPC/FHRestRequestRPCComponent.h"

void UFHRestRequest::NativeConstruct()
{
	Super::NativeConstruct();

	AFH_GS_LobbyGameState* GameState = Cast<AFH_GS_LobbyGameState>(UGameplayStatics::GetGameState(GetWorld()));
	if (GameState)
	{
		if (!GameState->OnLeftTimeReplicatedDelegate.Contains(this, TEXT("SetLeftTime")))
		{
			GameState->OnLeftTimeReplicatedDelegate.AddDynamic(this, &UFHRestRequest::SetLeftTime);
			GameState->OnTimerEndDelegate.AddDynamic(this, &UFHRestRequest::CloseWidget);
		}
		if (!GameState->GetLobbyState())
		{

		}
	}

	if (RPCComponent == nullptr)
	{
		APlayerController* LocalController = GetOwningPlayer();
		if (!LocalController)
		{
			return;
		}
		RPCComponent = LocalController->GetComponentByClass<UFHRestRequestRPCComponent>();
		//RPCComponent = NewObject<UFHRestRequestRPCComponent>(LocalController);
		//RPCComponent->RegisterComponent();
		//LocalController->AttachToComponent(LocalController->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
	}
}
