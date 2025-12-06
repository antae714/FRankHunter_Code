// Copyright F Rank Hunter. All Rights Reserved.


#include "Core/FHPropManager.h"
#include "Kismet/GamePlayStatics.h"

#include "Props/FHDisplayBoard.h"
#include "Props/FHCart.h"
#include "Props/FHKioskBase.h"
#include "Props/FHTray.h"
#include "ManagerActorRegistrySubsystem.h"

// Sets default values
AFHPropManager::AFHPropManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFHPropManager::BeginPlay()
{
	Super::BeginPlay();
	//CacheProps();
	//GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AFHPropManager::CacheProps);
}

void AFHPropManager::CacheProps()
{
	//TArray<AActor*> TaggedActors;
	//UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("InGameProps"), TaggedActors);

	//for (AActor* Actor : TaggedActors)
	//{
	//	if (AFHDisplayBoard* Sign = Cast<AFHDisplayBoard>(Actor))
	//	{
	//		Props.Add(TEXT("TestSign"), Sign);
	//	}
	//	else if (AFHCart* Cart = Cast<AFHCart>(Actor))
	//	{
	//		Props.Add(TEXT("Cart"), Cart);
	//	}
	//	else if (AFHKioskBase* Kiosk = Cast<AFHKioskBase>(Actor))
	//	{
	//		Props.Add(TEXT("Kiosk"), Kiosk);
	//	}
	//	else if (AFHTray* SellTray = Cast<AFHTray>(Actor))
	//	{
	//		Props.Add(TEXT("SellTray"), SellTray);
	//	}
	//}
}

// Called every frame
void AFHPropManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFHPropManager::RegistProp(FName PropName, AActor* Actor)
{
	Props.Add(PropName, Actor);
}

void AFHPropManager::UnRegistProp(FName PropName)
{
	if (AActor** Actor = Props.Find(PropName))
	{
		Props.Remove(PropName);
	}
}

AActor* AFHPropManager::BP_GetProp(const FName& PropKey)
{
	return GetProp<AActor>(PropKey);
}

