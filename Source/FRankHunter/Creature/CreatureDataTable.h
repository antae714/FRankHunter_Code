// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CreatureDataTable.generated.h"

// 나중에 
// 배보다 배꼽이 더큰듯 아마?

USTRUCT(BlueprintType)
struct FFHCreatuerData : public FTableRowBase
{
	GENERATED_BODY()

	virtual void OnPostDataImport(const UDataTable* InDataTable, const FName InRowName, TArray<FString>& OutCollectedImportProblems) override;
	virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName) override;
	void OnChangeItem(const FName InRowName);

	UPROPERTY()
	FName CreatureID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SightAngle = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SightRange = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HearingRange = 0.0f;
};


