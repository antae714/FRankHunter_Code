// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CommonActivatableWidget.h"
#include "FHSelectCharacterInfo.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHSelectCharacterInfo : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void IconStateReplicated(int32 Index, bool State);

	UFUNCTION(BlueprintImplementableEvent)
	void InitPawnState(const TArray<FName>& IconState);

	UFUNCTION(BlueprintImplementableEvent)
	void ShowSelectError();

	UFUNCTION(BlueprintImplementableEvent)
	void PawnSelected(FName PawnKey);
};
