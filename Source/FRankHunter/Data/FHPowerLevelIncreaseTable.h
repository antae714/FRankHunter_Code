// Copyright F Rank Hunter.. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "FHPowerLevelIncreaseTable.generated.h"

USTRUCT(BlueprintType)
struct FFHPowerLevelIncreaseTable : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IncreaseData")
	int32 Increase1 = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IncreaseData")
	int32 Increase2 = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IncreaseData")
	int32 Increase3 = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IncreaseData")
	int32 Increase4 = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="IncreaseData")
	int32 IncreasePerCoreDestroy = 0;
};