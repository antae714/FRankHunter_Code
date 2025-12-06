// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FHLevelUpExpRow.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FRANKHUNTER_API FFHLevelUpExpRow : public FTableRowBase
{
	GENERATED_BODY()
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Level = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 RequiredExp = 0;
};
