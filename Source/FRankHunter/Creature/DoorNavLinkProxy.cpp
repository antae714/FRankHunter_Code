// Copyright F Rank Hunter. All Rights Reserved.


#include "Creature/DoorNavLinkProxy.h"
#include "NavLinkCustomComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationSystem.h"

ADoorNavLinkProxy::ADoorNavLinkProxy()
{
	PointLinks.Empty();

	bSmartLinkIsRelevant = true;
	//SetSmartLinkEnabled(true);

	GetSmartLinkComp()->SetLinkData(FVector{-100.0f, 0.0f, 0.0f }, FVector{ 100.0f, 0.0f, 0.0f }, ENavLinkDirection::BothWays);
}

void ADoorNavLinkProxy::BeginPlay()
{
	Super::BeginPlay();

	OnSmartLinkReached.AddDynamic(this, &ThisClass::SmartLinkReached);
	if (auto* CAC = FindComponentByClass<UChildActorComponent>())
	{
		if (auto* Proxy = Cast<ANavLinkProxy>(CAC->GetChildActor()))
		{
			if (auto* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
			{
				NavSys->Build();
			}
		}
	}
}

void ADoorNavLinkProxy::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	Super::EndPlay(EndPlayReason);
}

void ADoorNavLinkProxy::SmartLinkReached(AActor* MovingActor, const FVector& DestinationPoint)
{
	AAIController* AIController = UAIBlueprintHelperLibrary::GetAIController(MovingActor);
	UPathFollowingComponent* PathFollowingComponent = AIController ? AIController->GetPathFollowingComponent() : nullptr;

	if (PathFollowingComponent)
	{
		PathFollowingComponent->PauseMove();

		ActorInUse = MovingActor;

		GetWorldTimerManager().SetTimer(ResumeTimer, this, &ThisClass::ResumeActorMove, 3.0f);
	}
}

void ADoorNavLinkProxy::ResumeActorMove()
{
	AAIController* AIController = UAIBlueprintHelperLibrary::GetAIController(ActorInUse);
	UPathFollowingComponent* PathFollowingComponent = AIController ? AIController->GetPathFollowingComponent() : nullptr;


	if (PathFollowingComponent)
	{
		PathFollowingComponent->ResumeMove();

	}
	ActorInUse = nullptr;
}
