// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FHLoading.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHLoading : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void Init();
	UFUNCTION(BlueprintImplementableEvent)
	void AddProgress(float Rate);
	UFUNCTION(BlueprintImplementableEvent)
	void SetProgress(float Rate);
};
