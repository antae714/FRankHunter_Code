// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Core/FRankHunterTypes.h"
#include "FHExplorationResult.generated.h"

class UFHTypingAnimationTextBlock;

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHExplorationResult : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void SetExplorationResult(const TArray<struct FPlayerInfo>& Players);

	UFUNCTION(BlueprintImplementableEvent)
	void ShowResult();
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ExplorateResult")
	int32 DiedPlayerCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ExplorationResult")
	int32 TotalPlayerCount = 0;

private:
	int32 CurrentPlayIndex = 0;
};
