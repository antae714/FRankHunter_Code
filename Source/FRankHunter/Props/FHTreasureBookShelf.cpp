// Copyright F Rank Hunter. All Rights Reserved.


#include "Props/FHTreasureBookShelf.h"
#include "BluePrintFunctions/FHBlueprintFunctionLibrary.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Component/FHInteractableComponent.h"
#include "Item/FHItemBase.h"


AFHTreasureBookShelf::AFHTreasureBookShelf()
{
	PrimaryActorTick.bCanEverTick = true;

	BookShelfMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BookShelfMesh"));
	SetRootComponent(BookShelfMesh);
	
	LeftTopAnchorArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("LeftTopAnchorArrow"));
	LeftTopAnchorArrow->SetupAttachment(BookShelfMesh);
}

void AFHTreasureBookShelf::BeginPlay()
{
	Super::BeginPlay();

	bDrawDebugArrow = false;

	if (HasAuthority())
	{
		SetupSpawingPoints();
		SpawnItems();
	}
}

void AFHTreasureBookShelf::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#ifdef WITH_EDITOR
	if (bDrawDebugArrow)
	{
		Debug_DrawSpawningPoints();
	}
#endif
}

void AFHTreasureBookShelf::Debug_DrawSpawningPoints()
{
	FVector Forward = GetActorForwardVector();
	FVector Right = GetActorRightVector();
	FVector InitAnchorPos = LeftTopAnchorArrow->GetComponentLocation();

	FVector LineStart;
	FVector LineEnd;
	float ArrowSize = 3.0f; // head size, maybe
	FColor ArrowColor = FColor::Red;
	bool isPersistant = false;
	float LifeTime = -1.0f; // 한 프레임?
	uint8 DepthPriority = 0;
	float Thickness = 2.0f;

	for (int32 row = 0; row < ShelfRow; ++row)
	{
		for (int32 col = 0; col < ShelfColumn; ++col)
		{
			if (PointException.Contains(FIntVector2{ row, col }))
			{
				continue;
			}

			LineStart = InitAnchorPos;
			LineStart -= Right * RowGap * row;
			LineStart.Z -= ColumnGap * col;

			LineEnd = LineStart + (Forward * 20.0f);

			DrawDebugDirectionalArrow(GetWorld(), LineStart, LineEnd, ArrowSize, ArrowColor, isPersistant, LifeTime, DepthPriority, Thickness);
		}
	}
}

void AFHTreasureBookShelf::SetupSpawingPoints()
{
	FVector Right = GetActorRightVector();
	FVector InitAnchorPos = LeftTopAnchorArrow->GetComponentLocation();
	FVector PointPos;

	for (int row = 0; row < ShelfRow; ++row)
	{
		for (int col = 0; col < ShelfColumn; ++col)
		{
			if (PointException.Contains(FIntVector2{ row, col }))
			{
				continue;
			}

			PointPos = InitAnchorPos;
			PointPos -= Right * RowGap * row;
			PointPos.Z -= ColumnGap * col;

			SpawningPoints.Add(PointPos);
		}
	}
}

void AFHTreasureBookShelf::SpawnItems()
{
	if (!Descriptor) return;
	if (SpawningPoints.IsEmpty()) return;

	FRotator ShelfRotation = GetActorRotation();

	int32 SpawnCount = GetRandomSpawnCount(Descriptor->MinimumItemNum, Descriptor->MaximumItemNum, Descriptor->Steepness);
	if (SpawningPoints.Num() < SpawnCount)
	{
		SpawnCount = SpawningPoints.Num();
	}

	uint32 CandNum = Descriptor->SpawnableActorList.Num();

	TArray<uint32> SpawningCounter;
	SpawningCounter.Init(0, CandNum);

	TArray<float> CandWeightCache;
	CandWeightCache.Init(0, CandNum);
	int _i = 0;
	for (FBookShelfSpawnableActorDescriptor& desc : Descriptor->SpawnableActorList)
	{
		if (desc.weights == 0.0f || desc.MaximumSpawnNum <= 0)
		{
			CandWeightCache[_i] = 0.0f;
		}
		else
		{
			CandWeightCache[_i] = desc.weights;
		}
		++_i;
	}

	for (int i = 0; i < SpawnCount; ++i)
	{
		// Select actor
		int32 actorIdx = UFHBlueprintFunctionLibrary::GetRandIndexUsingDiscreteDistribution(CandWeightCache);
		if (actorIdx == -1)
		{
			return;
		}
		FBookShelfSpawnableActorDescriptor& actorDesc = Descriptor->SpawnableActorList[actorIdx];
		
		SpawningCounter[actorIdx]++;
		if (actorDesc.MaximumSpawnNum <= SpawningCounter[actorIdx])
		{
			CandWeightCache[actorIdx] = 0.0f;
		}

		// Select spawn point
		uint32 pointIdx = FMath::RandRange(0, SpawningPoints.Num() - 1);
		FVector pointPos = SpawningPoints[pointIdx];
		SpawningPoints.RemoveAt(pointIdx);
		FRotator TargetRotation = actorDesc.bRandomRotation ? FRotator(0, FMath::RandRange(0.0f, 1.0f) * 360.0f, 0) : ShelfRotation;

		Blueprint_SpawnItem(pointPos, TargetRotation, actorDesc.item);

		/*FActorSpawnParameters params;
		params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		FRotator TargetRotation = actorDesc.bRandomRotation ? FRotator(0, FMath::RandRange(0.0f, 1.0f) * 360.0f, 0) : ShelfRotation;


		AActor* spawnedItem = GetWorld()->SpawnActor<AActor>(actorDesc.actor, pointPos, TargetRotation, params);

		UFHInteractableComponent* interactComp = spawnedItem->GetComponentByClass<UFHInteractableComponent>();
		interactComp->bIsHighlightable = true;

		UStaticMeshComponent* mesh = spawnedItem->GetComponentByClass<UStaticMeshComponent>();
		mesh->SetSimulatePhysics(false);*/
	}
}

int32 AFHTreasureBookShelf::GetRandomSpawnCount(int32 Min, int32 Max, float Steepness)
{
	if (Min > Max) return 0;
	if (Min == Max) return Min;

	TArray<float> weights;
	for (int32 i = Min; i <= Max; ++i)
	{
		weights.Add(1.0f / FMath::Pow(static_cast<float>(i - Min + 1), Steepness));
	}

	return Min + UFHBlueprintFunctionLibrary::GetRandIndexUsingDiscreteDistribution(weights);
}

