// Copyright F Rank Hunter. All Rights Reserved.


#include "DungeonGeneration/FHObjectSpawningPoint.h"
#include "FRankHunter.h"
#include "Components/ArrowComponent.h"
#include "Components/ChildActorComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DungeonGeneration/FHSpawningPointDescriptor.h"

void AFHObjectSpawningPoint::ShowPreviewActor()
{
	if (!SpawningPointDescriptor) return;
	if (!PreviewActor) return;

	if (previewIdx >= SpawningPointDescriptor->SpawnableActorList.Num())
	{
		previewIdx = 0;
		return;
	}
	TSubclassOf<AActor> target = SpawningPointDescriptor->SpawnableActorList[previewIdx].actor;
	PreviewActor->SetChildActorClass(target);
	//PreviewActor->SetWorldLocationAndRotation(GetActorLocation(), GetActorRotation());
	//PreviewActor->SetWorldScale3D(GetActorScale());

	++previewIdx;
	if (previewIdx >= SpawningPointDescriptor->SpawnableActorList.Num())
	{
		previewIdx = 0;
	}
}

//void AFHObjectSpawningPoint::OnConstruction(const FTransform& Transform)
//{
//	Super::OnConstruction(Transform);
//
//	if (!PreviewActor) return;
//	PreviewActor->SetWorldLocationAndRotation(GetActorLocation(), GetActorRotation());
//	PreviewActor->SetWorldScale3D(GetActorScale());
//}

void AFHObjectSpawningPoint::HidePreviewActor()
{
	if (!PreviewActor) return;
	PreviewActor->SetChildActorClass(nullptr);
}


AFHObjectSpawningPoint::AFHObjectSpawningPoint()
{
	PrimaryActorTick.bCanEverTick = false;

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	SetRootComponent(ArrowComponent);

	PreviewActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("PreviewActor"));
	PreviewActor->SetupAttachment(ArrowComponent);
}

void AFHObjectSpawningPoint::BeginPlay()
{
	Super::BeginPlay();

	PreviewActor->SetChildActorClass(nullptr);
}

void AFHObjectSpawningPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void AFHObjectSpawningPoint::SpawnObject()
{
	
}

