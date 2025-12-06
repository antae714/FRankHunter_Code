// Copyright F Rank Hunter. All Rights Reserved.


#include "UI/Voice/FHVoiceWidget.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/VoiceInterface.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "BlueprintDataDefinitions.h"

void UFHVoiceWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerBorders.Add(P1Border);
	PlayerBorders.Add(P2Border);

	IOnlineVoicePtr VoiceInterface = Online::GetVoiceInterface(GetWorld(), TEXT("Null"));
	if (VoiceInterface.IsValid())
	{
		//VoiceInterface->OnPlayerTalkingStateChangedDelegates.AddUObject(this, &UFHVoiceWidget::OnPlayerTalking);
	}
}

void UFHVoiceWidget::RegistLocalPlayer(const FUniqueNetIdRepl& UniqueNetId)
{
	PlayerTalkMap.Add(UniqueNetId, P1Border);
}

void UFHVoiceWidget::RegistRemotePlayer(const FUniqueNetIdRepl& UniqueNetId)
{
	PlayerTalkMap.Add(UniqueNetId, P2Border);
}

void UFHVoiceWidget::OnPlayerTalking(TSharedRef<const FUniqueNetId> UniqueNetIdRef, bool IsTalking)
{
	FUniqueNetIdRepl UniqueNetIdRepl(UniqueNetIdRef);

	TObjectPtr<UBorder>* Border = PlayerTalkMap.Find(UniqueNetIdRepl);
	if (Border)
	{
		FSlateBrush Brush = (*Border)->Background;
		FSlateColor OutlineColor = Brush.OutlineSettings.Color;
		FLinearColor LinearColor = OutlineColor.GetSpecifiedColor();
		if (IsTalking)
		{
			LinearColor.A = 1.0f;
		}
		else
		{
			LinearColor.A = 0.0f;
		}

		OutlineColor = LinearColor;
		Brush.OutlineSettings.Color = OutlineColor;

		(*Border)->SetBrush(Brush);
	}
}

void UFHVoiceWidget::OnPlayerTalking(const FBPUniqueNetId& UniqueNetID, bool IsTalking)
{
	const FUniqueNetId* NetId =	UniqueNetID.GetUniqueNetId();
	FUniqueNetIdRepl UniqueNetIdRepl(*NetId);

	TObjectPtr<UBorder>* Border = PlayerTalkMap.Find(UniqueNetIdRepl);
	if (Border)
	{
		FSlateBrush Brush = (*Border)->Background;
		FSlateColor OutlineColor = Brush.OutlineSettings.Color;
		FLinearColor LinearColor = OutlineColor.GetSpecifiedColor();
		if (IsTalking)
		{
			LinearColor.A = 1.0f;
		}
		else
		{
			LinearColor.A = 0.0f;
		}

		OutlineColor = LinearColor;
		Brush.OutlineSettings.Color = OutlineColor;

		(*Border)->SetBrush(Brush);
	}
}
