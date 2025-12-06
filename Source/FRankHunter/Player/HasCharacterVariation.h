// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HasCharacterVariation.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHasCharacterVariation : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FRANKHUNTER_API IHasCharacterVariation
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
	UObject* FindAssetData(FName VariationId) const;
	virtual UObject* FindAssetData_Implementation(FName VariationId) const;
};
