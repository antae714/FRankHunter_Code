// Copyright F Rank Hunter.. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FHGateSpecificationTable.generated.h"



USTRUCT(BlueprintType)
struct FGateThemeSpecificationSet
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ThemeName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Weight{ 1.0f };
};

USTRUCT(BlueprintType)
struct FFHGateSpecificationTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	FFHGateSpecificationTableRow() {}


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FGateThemeSpecificationSet> GateTheme;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MinRoomNum = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxRoomNum = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxMagicStoneValue = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxCreaturePower = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TimeLimit = 0;

	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MinRandomSpawnPropValue = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxRandomSpawnPropValue = 0;
};