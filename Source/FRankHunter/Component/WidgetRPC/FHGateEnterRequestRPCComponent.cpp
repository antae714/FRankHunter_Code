// Copyright F Rank Hunter. All Rights Reserved.


#include "Component/WidgetRPC/FHGateEnterRequestRPCComponent.h"
#include "Lobby/FHLobbyGameMode.h"
#include "BluePrintFunctions/FHBlueprintFunctionLibrary.h"
#include "UI/Lobby/FH_W_GateEnterRequest.h"
#include "Core/FHUIManager.h"

// Sets default values for this component's properties
UFHGateEnterRequestRPCComponent::UFHGateEnterRequestRPCComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UFHGateEnterRequestRPCComponent::OnAnswerEnterRequest_Implementation(bool Answer)
{
	APlayerController* Owner = GetOwner<APlayerController>();
	check(Owner);

	AFHLobbyGameMode* GameMode = Cast<AFHLobbyGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->AnswerToRequest(Answer, Owner);
	}
}

void UFHGateEnterRequestRPCComponent::OnPlayerAnserRequest_Implementation(const FString& PlayerName)
{
	AFHUIManager* UIManager = UFHBlueprintFunctionLibrary::GetUIManager(GetWorld());
	if (!UIManager)
	{
		return;
	}
	UFH_W_GateEnterRequest* Widget = UIManager->GetWidget<UFH_W_GateEnterRequest>(TEXT("EnterRequest"));
	if (!Widget)
	{
		return;
	}
	Widget->OnPlayerAnswer(PlayerName);
}
