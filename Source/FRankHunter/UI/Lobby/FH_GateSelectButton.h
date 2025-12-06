// Copyright F Rank Hunter.. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "FH_GateSelectButton.generated.h"

enum class EGateRank : uint8;

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFH_GateSelectButton : public UButton
{
	GENERATED_BODY()

public:
	UFH_GateSelectButton();

	UFUNCTION(BlueprintCallable)
	void OnButtonClicked();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GateInfo")
	int32 AreaID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GateInfo")
	EGateRank GateGrade;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Tooltip")
	TObjectPtr<class UFH_UW_GateTooltipBase> GateTooltip;

};
