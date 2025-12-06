// Copyright F Rank Hunter. All Rights Reserved.


#include "Props/Traps/FHTrapBase.h"
#include "BluePrintFunctions/FHBlueprintFunctionLibrary.h"
#include "Core/FHPropManager.h"
#include "FRankHunter.h"

// Sets default values
AFHTrapBase::AFHTrapBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

// Called when the game starts or when spawned
void AFHTrapBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFHTrapBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

