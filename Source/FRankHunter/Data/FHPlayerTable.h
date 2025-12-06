// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FHPlayerTable.generated.h"

USTRUCT(BlueprintType)
struct FFHPlayerSuitColorRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLinearColor Color;
};