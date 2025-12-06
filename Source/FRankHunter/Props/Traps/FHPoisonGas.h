// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Props/Traps/FHTrapBase.h"
#include "FHPoisonGas.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API AFHPoisonGas : public AFHTrapBase
{
	GENERATED_BODY()
	
public:


protected:


public:
	UFUNCTION(BlueprintCallable)
	void SetDensity(float Value);
};
