// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FHInteractTooltipWidget.generated.h"

class UCommonActionWidget;

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHInteractTooltipWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint32 bCalculateCursorPosition : 1{ false };

	UFUNCTION(BlueprintImplementableEvent)
	void SetInteractTooltipText(const TArray<FText>& TextArray, const TArray<UInputAction*>& InputActions);
};
