// Copyright F Rank Hunter. All Rights Reserved.


#include "UI/FHLobbyNPCWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/Border.h"

#include "Core/FHPlayerController.h"
#include "Player/FHPlayerBase.h"

#include "NPC/FHLobbyNPC.h"

#include "Kismet/GameplayStatics.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "BluePrintFunctions/FHBlueprintFunctionLibrary.h"
#include "UI/Lobby/FHBuyUIBase.h"

void UFHLobbyNPCWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetIsFocusable(true);

	//PayFeeBtn->OnClicked.AddDynamic(this, &UFHLobbyNPCWidget::PayFee);
	//WhatBtn->OnClicked.AddDynamic(this, &UFHLobbyNPCWidget::Sell);

	BuyUI->OnHomeButtonClickedDelegate = FOnHomeButtonClicked::CreateUObject(this, &UFHLobbyNPCWidget::BackToHome);
}

FReply UFHLobbyNPCWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
	//if (!CancelAction || !InputMappingContext) return Super::NativeOnKeyDown(InGeometry, InKeyEvent);

	//const TArray<FKey>& matchingKeys = UFHBlueprintFunctionLibrary::FindMatchingKeysInInputMappingContext(InputMappingContext, CancelAction);
	//if (matchingKeys.Contains(InKeyEvent.GetKey()))
	//{
	//	OnEscapeKeyPressed();
	//	// 이벤트를 여기서 처리했음을 알려 다른 위젯이나 액터가 이 입력을 더 이상 처리하지 않도록 함
	//	return FReply::Handled();
	//}

 //   return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UFHLobbyNPCWidget::PayFee()
{

}

void UFHLobbyNPCWidget::Sell()
{

}

void UFHLobbyNPCWidget::OnEscapeKeyPressed()
{
	if (WidgetMaster)
	{
		//WidgetMaster->OnFocusOut();
	}
}
