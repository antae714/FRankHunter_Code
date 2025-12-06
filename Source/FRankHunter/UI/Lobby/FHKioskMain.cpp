// Copyright F Rank Hunter. All Rights Reserved.


#include "FHKioskMain.h"
#include "FHShopBase.h"
#include "FHBuyUIBase.h"
#include "FHSellUIBase.h"
#include "LobbyUIUtils.h"

#include "Core/FHPlayerController.h"
#include "Core/FHUIManager.h"
#include "Player/FHPlayerBase.h"
#include "Kismet/GameplayStatics.h"

void UFHKioskMain::NativeConstruct()
{
	Super::NativeConstruct();

	BuyUI->OnHomeButtonClickedDelegate = FOnHomeButtonClicked::CreateUObject(this, &UFHKioskMain::BackToHome);
	SellUI->OnHomeButtonClickedDelegate = FOnHomeButtonClicked::CreateUObject(this, &UFHKioskMain::BackToHome);
}

void UFHKioskMain::OnExitButtonClicked()
{
	AFHPlayerController* PC = Cast<AFHPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (!PC)
	{
		return;
	}

	//AFHPlayerBase* PlayerBase = Cast<AFHPlayerBase>(PC->GetPawn());
	//if (PlayerBase)
	//{
	//	PlayerBase->SetAllowInteract(true);
	//	PlayerBase->EndFocusing();
	//}
}
