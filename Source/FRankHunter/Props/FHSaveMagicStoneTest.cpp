// Copyright F Rank Hunter. All Rights Reserved.


#include "Props/FHSaveMagicStoneTest.h"
#include "Component/FHInteractableComponent.h"

// Sets default values
AFHSaveMagicStoneTest::AFHSaveMagicStoneTest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	InteractableComp = CreateDefaultSubobject<UFHInteractableComponent>(TEXT("InteractableComp"));
}

// Called when the game starts or when spawned
void AFHSaveMagicStoneTest::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFHSaveMagicStoneTest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
