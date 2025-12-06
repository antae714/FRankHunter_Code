// Copyright F Rank Hunter. All Rights Reserved.


#include "FHPaymentBase.h"

#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Slider.h"
#include "Lobby/FH_GS_LobbyGameState.h"

void UFHPaymentBase::OpenUI()
{
	AFH_GS_LobbyGameState* GameState = Cast<AFH_GS_LobbyGameState>(GetWorld()->GetGameState());
	if (GameState)
	{
		int32 PlayerMoney = GameState->GetMoney();
		int32 GoalMoney = GameState->RequiredMoney;
		NeedToPay = GoalMoney - GameState->GetMoney();

		MoneyCanUse->SetText(FText::FromString(FString::FromInt(PlayerMoney)));
		PayGoal->SetText(FText::FromString(FString::FromInt(GoalMoney)));
		RemainingPayment->SetText(FText::FromString(FString::FromInt(NeedToPay)));

		int32 MinValue = PlayerMoney < NeedToPay ? PlayerMoney : NeedToPay;
		PaySlider->SetMaxValue(MinValue);
		MaxPlayerMoney->SetText(FText::FromString(FString::FromInt(MinValue)));
	}
}
