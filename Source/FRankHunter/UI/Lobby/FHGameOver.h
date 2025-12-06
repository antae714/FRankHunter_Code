// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FHGameOver.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHGameOver : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void Show();
};
