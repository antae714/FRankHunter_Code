// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetActionUtility.h"
#include "CreatureAssetActionUtility.generated.h"


UCLASS()
class FRANKHUNTEREDITORMODULE_API UFHAssetActionUtility : public UAssetActionUtility
{
	GENERATED_BODY()

public:
	UFHAssetActionUtility();


	UFUNCTION(CallInEditor)
	void SettingConstructionScriptData();
};


/**
 * 
 */
UCLASS()
class FRANKHUNTEREDITORMODULE_API UCreatureAssetActionUtility : public UAssetActionUtility
{
	GENERATED_BODY()

public:
	UCreatureAssetActionUtility();

	UFUNCTION(CallInEditor)
	void ExportToDataAsset(TSubclassOf<UDataAsset> DataAssetClass);

	UFUNCTION(CallInEditor)
	void MigrateToDataAsset(UDataAsset* DataAsset);

};
