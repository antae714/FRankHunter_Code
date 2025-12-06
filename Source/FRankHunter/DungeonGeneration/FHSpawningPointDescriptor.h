// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "FHSpawningPointDescriptor.generated.h"


USTRUCT(BlueprintType)
struct FRANKHUNTER_API FSpawnableActorDescriptor
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	uint32 bSpawnable : 1{ true };
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> actor;
	UPROPERTY(EditAnywhere)
	uint32 weights{ 1 };
	UPROPERTY(EditAnywhere)
	uint32 bRandomRotation : 1{ false };
};

UCLASS()
class FRANKHUNTER_API UFHSpawningPointDescriptor : public UDataAsset
{
	GENERATED_BODY()
	

public:
	// 1 to absolutely spawn, 0 to absolutely not spawn
	UPROPERTY(EditAnywhere, Category = "SpawningPointSetting")
	float SpawnChance;

	// Use this table when SpawnPoint catch the spawn chance
	// Do not use this property for spawning creatures.
	UPROPERTY(EditAnywhere, Category = "SpawingPointSetting")
	TArray<FSpawnableActorDescriptor> SpawnableActorList;


public:
	UFHSpawningPointDescriptor();
};
