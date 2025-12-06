// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FHQuotaNotice.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHQuotaNotice : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void Show();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MoneyUnit")
	FText MoneyUnit;
};
