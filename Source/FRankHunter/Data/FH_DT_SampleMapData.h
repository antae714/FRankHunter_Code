// Copyright F Rank Hunter.. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/FRankHunterTypes.h"
#include "FH_DT_SampleMapData.generated.h"

USTRUCT(BlueprintType)
struct FFHGateSampleData : public FTableRowBase
{
	GENERATED_BODY()

public:
	FFHGateSampleData()  
	  : ButtonOffsetX(-1),
		ButtonOffsetY(-1),
		AdjacentNodeValue(-1)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GateData")
	int32 ButtonOffsetX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GateData")
	int32 ButtonOffsetY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GateData")
	FString AreaName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GateData")
	int32 AdjacentNodeValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GateData")
	FString AreaInfo;
};

USTRUCT(BlueprintType)
struct FGradeID : public FTableRowBase
{
	GENERATED_BODY()

public:
	FGradeID()
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GradeData")
	FString Grade;

};

USTRUCT(BlueprintType)
struct FAreaUIData : public FTableRowBase
{
	GENERATED_BODY()

public:
	FAreaUIData()
		: ButtonOffsetX(0), ButtonOffsetY(0), AdjacentNodeValue(0)
	{ }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UIData")
	int32 ButtonOffsetX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UIData")
	int32 ButtonOffsetY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UIData")
	int32 AdjacentNodeValue;

};

USTRUCT(BlueprintType)
struct FAreaAdjacentData : public FTableRowBase
{
	GENERATED_BODY()

public:
	FAreaAdjacentData()
		: AdjacentNodeValue(0)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UIData")
	int32 AdjacentNodeValue;

};

USTRUCT(BlueprintType)
struct FGateGenerationRule : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GateRule")
	int32 MapSizeX = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GateRule")
	int32 MapSizeY = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GateRule")
	int32 RoomCountMin = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GateRule")
	int32 RoomCountMax = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GateRule")
	int32 MinPathRoomCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GateRule")
	int32 CoreCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GateRule")
	int32 TimeLimitSec = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GateRule")
	int32 ManaStoneCountMin = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GateRule")
	int32 ManaStoneCountMax = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GateRule")
	int32 ItemSpawnCountMin = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GateRule")
	int32 ItemSpawnCountMax = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GateRule")
	int32 MaxCreaturePower = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GateRule")
	TArray<FString> Creatures;

};

USTRUCT(BlueprintType)
struct FRankUpgradeTable : public FTableRowBase
{
	GENERATED_BODY()

public:
	FRankUpgradeTable();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rank")
	ELicenseRank CurrentRank;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rank")
	ELicenseRank NextRank;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rank")
	int32 CoreCount;
};

USTRUCT(BlueprintType)
struct FFHDialogTable : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dialog")
	FName DialogID;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dialog")
	FText DialogText;

};