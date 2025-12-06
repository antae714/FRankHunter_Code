// Copyright F Rank Hunter. All Rights Reserved.


#include "Component/WidgetRPC/FHSelectCharacterRPC.h"
#include "Lobby/FHLobbyGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "BlueprintFunctions/FHBlueprintFunctionLibrary.h"
#include "Core/FHUIManager.h"
#include "UI/Lobby/FHSelectCharacterInfo.h"

void UFHSelectCharacterRPC::PawnSelected_Implementation(FName PawnKey)
{
	AFHUIManager* UIManager = UFHBlueprintFunctionLibrary::GetUIManager(GetWorld());
	if (!UIManager)
	{
		return;
	}
	UFHSelectCharacterInfo* Widget = UIManager->GetWidget<UFHSelectCharacterInfo>(TEXT("SelectCharacter"));
	if (!Widget)
	{
		return;
	}
	Widget->PawnSelected(PawnKey);
}

void UFHSelectCharacterRPC::IconSlotReleased_Implementation(int32 Index)
{
	AFHLobbyGameMode* GameMode = Cast<AFHLobbyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GameMode)
	{
		return;
	}
	GameMode->SlotReleaseSelect(Index);
}

void UFHSelectCharacterRPC::IconStateReplicated_Implementation(int32 Index, bool State)
{
	AFHUIManager* UIManager = UFHBlueprintFunctionLibrary::GetUIManager(GetWorld());
	if (!UIManager)
	{
		return;
	}
	UFHSelectCharacterInfo* Widget = UIManager->GetWidget<UFHSelectCharacterInfo>(TEXT("SelectCharacter"));
	if (!Widget)
	{
		return;
	}
	Widget->IconStateReplicated(Index, State);
}

void UFHSelectCharacterRPC::InitPawnState_Implementation(const TArray<FName>& StateArray)
{
	AFHUIManager* UIManager = UFHBlueprintFunctionLibrary::GetUIManager(GetWorld());
	if (!UIManager)
	{
		return;
	}
	UFHSelectCharacterInfo* Widget = UIManager->GetWidget<UFHSelectCharacterInfo>(TEXT("SelectCharacter"));
	if (!Widget)
	{
		return;
	}
	Widget->InitPawnState(StateArray);
	Widget->SetVisibility(ESlateVisibility::Visible);
	Widget->ActivateWidget();

}

void UFHSelectCharacterRPC::CharacterSelectComplete_Implementation(AActor* MeshPreviewActor, const FName& ActorKey, const FName& SkillID)
{
	AFHLobbyGameMode* GameMode = Cast<AFHLobbyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GameMode)
	{
		return;
	}
	
	GameMode->PlayerSelectCompleted(GetOwner<APlayerController>(), MeshPreviewActor, ActorKey, SkillID);
}

void UFHSelectCharacterRPC::SelectError_Implementation()
{
	AFHUIManager* UIManager = UFHBlueprintFunctionLibrary::GetUIManager(GetWorld());
	if (!UIManager)
	{
		return;
	}
	UFHSelectCharacterInfo* Widget = UIManager->GetWidget<UFHSelectCharacterInfo>(TEXT("SelectCharacter"));
	if (!Widget)
	{
		return;
	}
	Widget->ShowSelectError();
}
