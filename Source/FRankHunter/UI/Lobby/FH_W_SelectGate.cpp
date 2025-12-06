// Copyright F Rank Hunter.. All Rights Reserved.


#include "FH_W_SelectGate.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "FH_GateSelectButton.h"
#include "GameFramework/PlayerController.h"
#include "Algo/RandomShuffle.h"
#include "Kismet/GameplayStatics.h"
#include "Data/FH_DT_SampleMapData.h"
#include "Components/CanvasPanelSlot.h"
#include "Core/FHPlayerStateBase.h"
#include "Lobby/FH_GS_LobbyGameState.h"
#include "DungeonGeneration/FHGateDefines.h"

void UFH_W_SelectGate::NativeConstruct()
{
	Super::NativeConstruct();

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	GateButtons.Add(GateSelectE);
	GateButtons.Add(GateSelectD);
	GateButtons.Add(GateSelectC);
	GateButtons.Add(GateSelectB);
	GateButtons.Add(GateSelectA);
	GateButtons.Add(GateSelectS);
	//for (int i = 0; i < GateButtons.Num(); i++) 
	//{
	//	GateButtons[i]->GateTooltip = GateTooltip;
	//}
}

void UFH_W_SelectGate::PostLoad()
{
	Super::PostLoad();
	

}

void UFH_W_SelectGate::ResetGateSelectPannel(const TArray<uint8>& SelectedGateInfo)
{
	UE_LOG(LogTemp, Warning, TEXT("SelectGate Local.."));

	for (int i = 0; i < GateButtons.Num(); i++)
	{
		GateButtons[i]->SetVisibility(ESlateVisibility::Collapsed);
	}

	int32 Index = 0;
	for (uint8 SelectedGateData : SelectedGateInfo)
	{
		uint8 AreaID, GateGrade;

		AreaID = SelectedGateData & 0x1F;
		GateGrade = (SelectedGateData >> 5) & 0x07;

		switch (EGateRank(GateGrade))
		{
		case EGateRank::E:
			GateSelectE->AreaID = AreaID;
			SetButtonLocation(GateSelectE, AreaID);
			break;
		case EGateRank::D:
			GateSelectD->AreaID = AreaID;
			SetButtonLocation(GateSelectD, AreaID);
			break;
		case EGateRank::C:
			GateSelectC->AreaID = AreaID;
			SetButtonLocation(GateSelectC, AreaID);
			break;
		case EGateRank::B:
			GateSelectB->AreaID = AreaID;
			SetButtonLocation(GateSelectB, AreaID);
			break;
		case EGateRank::A:
			GateSelectA->AreaID = AreaID;
			SetButtonLocation(GateSelectA, AreaID);
			break;
		case EGateRank::S:
			GateSelectS->AreaID = AreaID;
			SetButtonLocation(GateSelectS, AreaID);
			break;
		}
		//GateButtons[Index]->SetVisibility(ESlateVisibility::Visible);
		Index++;
	}
}

void UFH_W_SelectGate::SetButtonLocation(TObjectPtr<UFH_GateSelectButton> GateButton, int32 AreaID)
{
	FAreaUIData* UIData = AreaUIData->FindRow<FAreaUIData>(FName(*(FString::FromInt(AreaID))), TEXT("AreaUIData"));

	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(GateButton->Slot);
	if (CanvasSlot)
	{
		CanvasSlot->SetPosition(FVector2D(UIData->ButtonOffsetX, UIData->ButtonOffsetY));
	}
}
