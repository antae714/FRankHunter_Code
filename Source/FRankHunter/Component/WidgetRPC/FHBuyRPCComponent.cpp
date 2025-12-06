// Copyright F Rank Hunter. All Rights Reserved.


#include "Component/WidgetRPC/FHBuyRPCComponent.h"
#include "UI/Lobby/FHBuyUIBase.h"
#include "Lobby/FHLobbyGameMode.h"

// Sets default values for this component's properties
UFHBuyRPCComponent::UFHBuyRPCComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UFHBuyRPCComponent::PurchaseItem_Implementation(const TArray<struct FPurchaseData>& ItemData, int32 TotalPrice)
{
	UE_LOG(LogTemp, Warning, TEXT("Total Pruchase Price: %d"), TotalPrice);

	for (FPurchaseData PurchaseData : ItemData)
	{
		UE_LOG(LogTemp, Warning, TEXT("Purchase Item Name: %s, Purchase Count: %d"), *(PurchaseData.ItemID.ToString()), PurchaseData.ItemCount);
	}

	APlayerController* Owner = GetOwner<APlayerController>();
	check(Owner);

	AFHLobbyGameMode* LobbyGameMode = Cast<AFHLobbyGameMode>(GetWorld()->GetAuthGameMode());
	if (LobbyGameMode)
	{
		LobbyGameMode->PurchaseItem(Owner, ItemData, TotalPrice);
	}
}
