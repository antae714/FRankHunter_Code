// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FHTreasureBookShelf.generated.h"


USTRUCT(BlueprintType)
struct FRANKHUNTER_API FBookShelfSpawnableActorDescriptor
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UFHItemBase> item;
	UPROPERTY(EditAnywhere)
	float weights{ 1 };
	UPROPERTY(EditAnywhere)
	uint32 bRandomRotation : 1{ false };
	UPROPERTY(EditAnywhere)
	uint32 MaximumSpawnNum{ 1 };
};

UCLASS()
class FRANKHUNTER_API UFHTreasureBookShelfSpawningDescriptor : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	int32 MinimumItemNum{ 0 };
	UPROPERTY(EditAnywhere)
	int32 MaximumItemNum{ 4 };

	// If you higher the facter, fewer items will spawn.
	// It is recommended to set this factor as between 1.0 to 2.0
	UPROPERTY(EditAnywhere)
	float Steepness{ 1.5f };

	UPROPERTY(EditAnywhere)
	TArray<FBookShelfSpawnableActorDescriptor> SpawnableActorList;
};

UCLASS()
class FRANKHUNTER_API AFHTreasureBookShelf : public AActor
{
	GENERATED_BODY()
	

public:
	UPROPERTY(EditAnywhere, Category = "TreasureBookShelf")
	TObjectPtr<UFHTreasureBookShelfSpawningDescriptor> Descriptor;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UStaticMeshComponent> BookShelfMesh;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UArrowComponent> LeftTopAnchorArrow;
	UPROPERTY(EditAnywhere, Category = "TreasureBookShelf")
	int32 ShelfRow{ 5 };
	UPROPERTY(EditAnywhere, Category = "TreasureBookShelf")
	int32 ShelfColumn{ 5 };
	UPROPERTY(EditAnywhere, Category = "TreasureBookShelf")
	float RowGap{ 40.0f };
	UPROPERTY(EditAnywhere, Category = "TreasureBookShelf")
	float ColumnGap{ 20.0f };

	UPROPERTY(EditAnywhere, Category = "TreasureBookShelf")
	TSet<FIntVector2> PointException;

	TArray<FVector> SpawningPoints;

public:	
	AFHTreasureBookShelf();

protected:
	bool bDrawDebugArrow{ true };
	virtual void BeginPlay() override;
public:
	virtual void Tick(float DeltaTime) override;
	// Allows us to draw debug shapes in the blueprint editor
	virtual bool ShouldTickIfViewportsOnly() const override
	{
		return true;
	}

protected:
	void Debug_DrawSpawningPoints();

	void SetupSpawingPoints();
	void SpawnItems();
	int32 GetRandomSpawnCount(int32 Min, int32 Max, float Steepness);

public:
	UFUNCTION(BlueprintImplementableEvent)
	void Blueprint_SpawnItem(FVector Position, FRotator Rotation, TSubclassOf<class UFHItemBase> Item);
};
