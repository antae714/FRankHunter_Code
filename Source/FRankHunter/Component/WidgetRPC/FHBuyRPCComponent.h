// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Component/FHRPCComponent.h"
#include "FHBuyRPCComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FRANKHUNTER_API UFHBuyRPCComponent : public UFHRPCComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFHBuyRPCComponent();

	UFUNCTION(Server, Reliable)
	void PurchaseItem(const TArray<struct FPurchaseData>& ItemData, int32 TotalPrice);
	void PurchaseItem_Implementation(const TArray<struct FPurchaseData>& ItemData, int32 TotalPrice);

	virtual FName GetComponentName() { return TEXT("UFHBuyRPC"); }
};
