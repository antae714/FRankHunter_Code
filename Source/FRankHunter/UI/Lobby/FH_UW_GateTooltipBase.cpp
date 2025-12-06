// Copyright F Rank Hunter.. All Rights Reserved.


#include "FH_UW_GateTooltipBase.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Engine/DataTable.h"
#include "Data/FH_DT_SampleMapData.h"
#include "Core/FHPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Component/WidgetRPC/FHGateTooltipRPCComponent.h"

void UFH_UW_GateTooltipBase::NativeConstruct()
{
	Super::NativeConstruct();

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		if (PC->IsLocalController() && PC->HasAuthority())
		{
			if (GateEnter)
			{
				GateEnter->SetVisibility(ESlateVisibility::Visible);
			}
		}
		else
		{
			if (GateEnter)
			{
				GateEnter->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}
}

void UFH_UW_GateTooltipBase::PostLoad()
{
	Super::PostLoad();


}

#include "Lobby/FH_GS_LobbyGameState.h"
#include "DungeonGeneration/FHGateDefines.h"

void UFH_UW_GateTooltipBase::UpdateMapData(const EGateRank& GateGrade)
{
	AFH_GS_LobbyGameState* GameState = Cast<AFH_GS_LobbyGameState>(UGameplayStatics::GetGameState(GetWorld()));
	if (!GameState)
	{
		return;
	}

	GateRank = GateGrade;

	int32 LicenseRankInt = (int32)GameState->TeamLicenseRank;
	int32 MaxGateRank = FMath::Min(LicenseRankInt + 1, (int32)EGateRank::S);
	int32 MinGateRank = FMath::Max(LicenseRankInt - 1, (int32)EGateRank::E);

	if (GetOwningPlayer()->HasAuthority())
	{
		GateEnter->SetVisibility(ESlateVisibility::Visible);
		if ((int32)GateRank > MaxGateRank)
		{
			BP_SetEnterState(false);
		}
		else
		{
			BP_SetEnterState(true);
		}
	}
	else
	{
		BP_SetEnterState(false);
	}

	BP_UpdateMapData(GateGrade);
}

