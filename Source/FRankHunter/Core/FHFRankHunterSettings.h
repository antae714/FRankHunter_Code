// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Item/ItemTypes.h"
#include "FHFRankHunterSettings.generated.h"

/**
 * 
 */
UCLASS(config = Game, defaultconfig, meta = (DisplayName = "FRankHunter"))
class FRANKHUNTER_API UFHFRankHunterSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	FName GetLocalizeStringTableID() const;

public:
	UPROPERTY(EditAnywhere, config, Category = "Item", meta = (RequiredAssetDataTags = "RowStructure=/Script/FRankHunter.FHItemData"))
	TSoftObjectPtr<UDataTable> ItemDataTable;

	UPROPERTY(EditAnywhere, config, Category = "Item", meta = (RequiredAssetDataTags = "RowStructure=/Script/FRankHunter.FHThrowItemData"))
	TSoftObjectPtr<UDataTable> ThrowableItemDataTable;

	UPROPERTY(EditAnywhere, config, Category = "Item", meta = (RequiredAssetDataTags = "RowStructure=/Script/FRankHunter.FHDeployItemData"))
	TSoftObjectPtr<UDataTable> DeployItemDataTable;

	UPROPERTY(EditAnywhere, config, Category = "Item")
	TSoftObjectPtr<UDataAsset> ItemClassDataAsset;

	UPROPERTY(EditAnywhere, config, Category = "Item", meta = (RequiredAssetDataTags = "RowStructure=/Script/FRankHunter.FHItemDrobTableRow"))
	TSoftObjectPtr<UDataTable> ItemDropTable;

	UPROPERTY(EditAnywhere, config, Category = "Item", meta = (RequiredAssetDataTags = "RowStructure=/Script/FRankHunter.FHManaStoneDropTableRow"))
	TSoftObjectPtr<UDataTable> ManaStoneDropTable;

	UPROPERTY(EditAnywhere, config, Category = "Item|Color")
	FItemRarityColorArray ItemRarityColors;
	
	UPROPERTY(EditAnywhere, config, Category = "Player", meta = (RequiredAssetDataTags = "RowStructure=/Script/FRankHunter.FHLevelUpExpRow"))
	TSoftObjectPtr<UDataTable> LevelUpExpTable;

	UPROPERTY(EditAnywhere, config, Category = "Localize")
	TSoftObjectPtr<UStringTable> LocalizedStringTable;


	UPROPERTY(EditAnywhere, config, Category = "Sound", meta = (RequiredAssetDataTags = "RowStructure=/Script/FRankHunter.FHSoundDataTableRow"))
	TSoftObjectPtr<UDataTable> SoundDataTable;

	UPROPERTY(EditAnywhere, config, Category = "Level")
	TSoftObjectPtr<UDataAsset> LevelAsset;
	
	UPROPERTY(EditAnywhere, config, Category = "Skill", meta = (RequiredAssetDataTags = "RowStructure=/Script/FRankHunter.FHSkillTable"))
	TSoftObjectPtr<UDataTable> SkillDataTable;

	UPROPERTY(EditAnywhere, config, Category = "DataAsset")
	TSoftObjectPtr<UDataAsset> SelectInfoIconDataAsset;

	UPROPERTY(EditAnywhere, config, Category = "DataAsset")
	TSoftObjectPtr<UDataAsset> GateThemaAsset;
};
