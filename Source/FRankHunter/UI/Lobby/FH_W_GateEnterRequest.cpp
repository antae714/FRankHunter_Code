// Copyright F Rank Hunter.. All Rights Reserved.


#include "FH_W_GateEnterRequest.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/SizeBox.h"
#include "Lobby/FH_GS_LobbyGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Component/WidgetRPC/FHGateEnterRequestRPCComponent.h"
#include "BluePrintFunctions/FHBlueprintFunctionLibrary.h"
#include "Core/FHSoundManagerActor.h"
#include "GAS/FHGameplayTags.h"


bool UFH_W_GateEnterRequest::Initialize()
{
	bool Result = Super::Initialize();

	PlayerTexts.Add(Player1);
	PlayerTexts.Add(Player2);
	PlayerTexts.Add(Player3);
	PlayerTexts.Add(Player4);

	if (EnterErrorBox)
	{
		EnterErrorBox->SetVisibility(ESlateVisibility::Hidden);
	}

	if (EnterRequestBox)
	{
		EnterRequestBox->SetVisibility(ESlateVisibility::Visible);
	}

	if (TimeOutBox)
	{
		TimeOutBox->SetVisibility(ESlateVisibility::Hidden);
	}
	CurIdIndex = 0;

	AFH_GS_LobbyGameState* GameState = Cast<AFH_GS_LobbyGameState>(UGameplayStatics::GetGameState(GetWorld()));
	if (GameState)
	{
		if (!GameState->OnLeftTimeReplicatedDelegate.Contains(this, TEXT("SetLeftTime")))
		{
			GameState->OnLeftTimeReplicatedDelegate.AddDynamic(this, &UFH_W_GateEnterRequest::SetLeftTime);
			GameState->OnTimerEndDelegate.AddDynamic(this, &UFH_W_GateEnterRequest::OpenTimeOutWidget);
		}
	}

	if (RPCComponent == nullptr)
	{
		APlayerController* LocalController = GetOwningPlayer();
		if (!LocalController)
		{
			return Result;
		}
		RPCComponent = LocalController->GetComponentByClass<UFHGateEnterRequestRPCComponent>();

		//RPCComponent = NewObject<UFHGateEnterRequestRPCComponent>(LocalController);
		//RPCComponent->RegisterComponent();
		//LocalController->AttachToComponent(LocalController->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
	}

	return Result;
}

void UFH_W_GateEnterRequest::SetLeftTime(int32 Time)
{
	LeftTime->SetText(FText::FromString(FString::FromInt(Time)));
}

void UFH_W_GateEnterRequest::SetRefusePlayerName(const FString& PlayerName)
{
	UE_LOG(LogTemp, Warning, TEXT("CurIndex: %d"), CurIdIndex);
	if (CurIdIndex >= 0 && CurIdIndex < 4)
	{
		PlayerTexts[CurIdIndex]->SetText(FText::FromString(PlayerName));
	}
}

//void UFH_W_GateEnterRequest::OpenAnswerWidget(const FString& GateName)
//{
//	if (EnterErrorBox)
//	{
//		EnterErrorBox->SetVisibility(ESlateVisibility::Hidden);
//	}
//
//	if (EnterRequestBox)
//	{
//		if (GateNameQuestion)
//		{
//			FString FinalGateNameQuestion = FString::Printf(TEXT("%s에 진입하시겠습니까?"), *GateName);
//			GateNameQuestion->SetText(FText::FromString(FinalGateNameQuestion));
//		}
//		EnterRequestBox->SetVisibility(ESlateVisibility::Visible);
//	}
//
//	if (TimeOutBox)
//	{
//		TimeOutBox->SetVisibility(ESlateVisibility::Hidden);
//	}
//
//	OnWidgetOpened();
//}

void UFH_W_GateEnterRequest::ResetRefusePlayerText()
{
	for(int32 i = 0; i < PlayerTexts.Num(); i++)
	{
		FString Blank = TEXT(" ");
		PlayerTexts[i]->SetText(FText::FromString(Blank));
	}
}

