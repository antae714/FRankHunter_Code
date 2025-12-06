// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/ManagerActor.h"
#include "DungeonGeneration/FHGateDefines.h"
#include "ItemDropManagerActor.generated.h"

class UFHInventoryComponent;
class AFHWorldItemActor;
struct FFHManaStoneDropTableRow;
class UFHItemBase;

UCLASS()
class FRANKHUNTER_API AItemDropManagerActor : public AManagerActor
{
	GENERATED_BODY()
	
public:	
	AItemDropManagerActor();
	virtual FName GetManagerName() const { return TEXT("ItemDropManagerActor"); }

	/* 아마 안쓸듯 */
	void SpawnManaStone(const FVector& SpawnPoint, TSubclassOf<AFHWorldItemActor> ActorClass = nullptr);

	TSubclassOf<UFHItemBase> GetSpawnManaStoneInfo();

protected:
	virtual void BeginPlay() override;

private:
	TArray<float> WeightArray;
	TArray<FName> ItemIDArray;


	UPROPERTY(EditAnywhere, Category = "ItemDropManagerActor")
	EGateRank TempGateRank;
};
