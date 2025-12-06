// Copyright F Rank Hunter. All Rights Reserved.


#include "Props/FHCoreDestroyTest.h"
#include "Core/FHGateGameModeBase.h"
#include "Player/FHPlayerBase.h"

#include "Component/FHInteractableComponent.h"

// Sets default values
AFHCoreDestroyTest::AFHCoreDestroyTest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InteractableComp = CreateDefaultSubobject<UFHInteractableComponent>(TEXT("InteractableComp"));

}

// Called when the game starts or when spawned
void AFHCoreDestroyTest::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		InteractableComp->OnInteractServer.AddDynamic(this, &AFHCoreDestroyTest::OnInteract_Impl);
	}
}

// Called every frame
void AFHCoreDestroyTest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFHCoreDestroyTest::OnInteract_Impl(class AFHPlayerBase* Player, const UInputAction* InputAction)
{
	Cast<AFHGateGameModeBase>(GetWorld()->GetAuthGameMode())->InformCoreDestroyed();
}
