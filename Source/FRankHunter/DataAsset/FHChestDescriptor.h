// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FHChestDescriptor.generated.h"

USTRUCT()
struct FChestSpawnSet
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UFHItemBase> Item;
	UPROPERTY(EditAnywhere)
	float Weight{ 1.0f };
};

UCLASS()
class FRANKHUNTER_API UFHChestDescriptor : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	uint8 bCanSpawnedAsChestTrap : 1{ false };
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bCanSpawnedAsChestTrap == true", EditConditionHides))
	float SpawnedAsChestTrapCance{ 0.1f };

	UPROPERTY(EditAnywhere)
	float LockChance{ 0.5f };
	UPROPERTY(EditAnywhere)
	uint8 bUseRandomNum : 1{ false };
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bUseRandomNum == false", EditConditionHides))
	int32 SpawnItemNum{ 1 };
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bUseRandomNum == true", EditConditionHides))
	int32 MinSpawnItemNum{ 1 };
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bUseRandomNum == true", EditConditionHides))
	int32 MaxSpawnItemNum{ 4 };

	
	UPROPERTY(EditAnywhere)
	uint32 bRandomRotationY : 1{ false };

	UPROPERTY(EditAnywhere)
	TArray<FChestSpawnSet> SpawnableList;
};
