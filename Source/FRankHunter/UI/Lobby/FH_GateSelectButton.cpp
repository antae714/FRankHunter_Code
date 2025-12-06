// Copyright F Rank Hunter.. All Rights Reserved.


#include "FH_GateSelectButton.h"
#include "FH_UW_GateTooltipBase.h"
#include "Data/FH_DT_SampleMapData.h"
#include "DungeonGeneration/FHGateDefines.h"


UFH_GateSelectButton::UFH_GateSelectButton()
	: AreaID(-1), GateGrade(EGateRank::Invalid)
{
}

void UFH_GateSelectButton::OnButtonClicked()
{
	if (GateTooltip)
	{
		UE_LOG(LogTemp, Warning, TEXT("Call GateTooltip. AreaID: %d, GateGrade: %d"), AreaID, GateGrade);
		GateTooltip->UpdateMapData(GateGrade);
	}
}
