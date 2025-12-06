// Copyright F Rank Hunter. All Rights Reserved.


#include "Props/Preview/FHPreviewActorBase.h"

// Sets default values
AFHPreviewActorBase::AFHPreviewActorBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFHPreviewActorBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFHPreviewActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFHPreviewActorBase::SetDisableActor_Implementation()
{
	DisableActor();
}

