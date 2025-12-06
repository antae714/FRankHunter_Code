// Copyright F Rank Hunter. All Rights Reserved.


#include "Props/FHPurchaseStorage.h"
#include "Core/FHPropManager.h"
#include "Core/ManagerActorRegistrySubsystem.h"

// Sets default values
AFHPurchaseStorage::AFHPurchaseStorage()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

// Called when the game starts or when spawned
void AFHPurchaseStorage::BeginPlay()
{
	Super::BeginPlay();
	
	UManagerActorRegistrySubsystem* Subsystem = GetWorld()->GetSubsystem<UManagerActorRegistrySubsystem>();
	AFHPropManager* PropManager = Subsystem->GetManagerActor<AFHPropManager>(TEXT("PropManager"));
	if (PropManager)
	{
		PropManager->RegistProp(TEXT("PurchaseStorage"), this);
	}
	else
	{
		Subsystem->GetManagerRegistDelegate(TEXT("PropManager")).AddDynamic(this, &AFHPurchaseStorage::NotifyPropManagerRegisted);
	}
}

// Called every frame
void AFHPurchaseStorage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFHPurchaseStorage::NotifyPropManagerRegisted()
{
	UManagerActorRegistrySubsystem* Subsystem = GetWorld()->GetSubsystem<UManagerActorRegistrySubsystem>();
	AFHPropManager* PropManager = Subsystem->GetManagerActor<AFHPropManager>(TEXT("PropManager"));
	PropManager->RegistProp(TEXT("PurchaseStorage"), this);
}

