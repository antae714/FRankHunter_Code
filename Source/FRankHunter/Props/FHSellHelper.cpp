// Copyright F Rank Hunter. All Rights Reserved.


#include "Props/FHSellHelper.h"
#include "Core/FHPropManager.h"
#include "Core/ManagerActorRegistrySubsystem.h"
#include "Component/FHInteractableComponent.h"

#include "Props/FHTray.h"

// Sets default values
AFHSellHelper::AFHSellHelper()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	InteractableComponent = CreateDefaultSubobject<UFHInteractableComponent>(TEXT("InteractableComponent"));
}

// Called when the game starts or when spawned
void AFHSellHelper::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFHSellHelper::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFHSellHelper::Interact_Impl(class AFHPlayerBase* Player)
{
	if (!IsValid(ConnectedTray))
	{
		return;
	}

	TurnOffInteract();

	ConnectedTray->ConnectedButton = this;
	ConnectedTray->ProcessSellItem();

	/*UManagerActorRegistrySubsystem* Subsystem = GetWorld()->GetSubsystem<UManagerActorRegistrySubsystem>();
	check(Subsystem);

	AFHPropManager* PropManager = Subsystem->GetManagerActor<AFHPropManager>(TEXT("PropManager"));
	check(PropManager);

	if (AFHTray* SellTray = PropManager->GetProp<AFHTray>(TEXT("SellTray")))
	{
		SellTray->ProcessSellItem();
	}*/
}

void AFHSellHelper::TurnOffInteract_Implementation()
{
	InteractableComponent->SetIsInteractEnable(false);
	InteractableComponent->SetIsTooltipUIEnable(false);
}

void AFHSellHelper::TurnOnInteract_Implementation()
{
	InteractableComponent->SetIsInteractEnable(true);
	InteractableComponent->SetIsTooltipUIEnable(true);
}

void AFHSellHelper::MakeInteractable()
{
	TurnOnInteract();
}

