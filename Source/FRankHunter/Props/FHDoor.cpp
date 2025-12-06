// Copyright F Rank Hunter. All Rights Reserved.


#include "Props/FHDoor.h"
#include "NavLinkCustomComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Props/FHDoorBase.h"
#include "AIController.h"
#include "Creature/FHCreatureBase.h"
#include "Components/BoxComponent.h"

AFHDoor::AFHDoor()
{
	SmartLinkComp = CreateDefaultSubobject<UNavLinkCustomComponent>(TEXT("SmartLinkComp"));
	SmartLinkComp->SetNavigationRelevancy(true);
	SmartLinkComp->SetLinkData(FVector{ -100.0f, 0.0f, 0.0f }, FVector{ 100.0f, 0.0f, 0.0f }, ENavLinkDirection::BothWays);

	LeftDoorActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("LeftDoorActor"));
	LeftDoorActor->SetupAttachment(GetRootComponent());
	RightDoorActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("RightDoorActor"));
	RightDoorActor->SetupAttachment(GetRootComponent());

	BoxForPreventingSpawningNearby = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxForPreventingSpawningNearby"));
	BoxForPreventingSpawningNearby->SetupAttachment(GetRootComponent());
	BoxForPreventingSpawningNearby->SetCollisionProfileName(TEXT("NoCollision"));
	BoxForPreventingSpawningNearby->SetCanEverAffectNavigation(false);
	BoxForPreventingSpawningNearby->SetGenerateOverlapEvents(false);
}

void AFHDoor::TurnOnBoxCollisionForPreventingSpawningNearby()
{
	if (BoxForPreventingSpawningNearby == nullptr) return;

	BoxForPreventingSpawningNearby->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	BoxForPreventingSpawningNearby->SetCanEverAffectNavigation(true);
}

void AFHDoor::TurnOffBoxCollisionForPreventingSpawningNearby()
{
	if (BoxForPreventingSpawningNearby == nullptr) return;

	BoxForPreventingSpawningNearby->SetCollisionProfileName(TEXT("NoCollision"));
	BoxForPreventingSpawningNearby->SetCanEverAffectNavigation(false);
}

void AFHDoor::BeginPlay()
{
	Super::BeginPlay();

	SmartLinkComp->SetMoveReachedLink(this, &ThisClass::NotifySmartLinkReached);

	AFHDoorBase* DoorBase1 = Cast<AFHDoorBase>(LeftDoorActor->GetChildActor());
	AFHDoorBase* DoorBase2 = Cast<AFHDoorBase>(RightDoorActor->GetChildActor());

	if (DoorBase1 && DoorBase2)
	{
		DoorBase1->PairDoor = DoorBase2;
		DoorBase2->PairDoor = DoorBase1;
	}
	else
	{
		LeftDoorActor->OnChildActorCreated().AddUObject(this, &ThisClass::OnDoorCreated);
		RightDoorActor->OnChildActorCreated().AddUObject(this, &ThisClass::OnDoorCreated);
	}

}

void AFHDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsInit)
	{
		AFHDoorBase* DoorBase1 = Cast<AFHDoorBase>(LeftDoorActor->GetChildActor());
		AFHDoorBase* DoorBase2 = Cast<AFHDoorBase>(RightDoorActor->GetChildActor());

		if (DoorBase1 && DoorBase2)
		{
			DoorBase1->PairDoor = DoorBase2;
			DoorBase2->PairDoor = DoorBase1;
			bIsInit = true;
		}
	}
}

void AFHDoor::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	Super::EndPlay(EndPlayReason);
}

void AFHDoor::NotifySmartLinkReached(UNavLinkCustomComponent* LinkComp, UObject* PathingAgent, const FVector& DestPoint)
{
	UPathFollowingComponent* PathComp = Cast<UPathFollowingComponent>(PathingAgent);
	AFHDoorBase* DoorBase1 = Cast<AFHDoorBase>(LeftDoorActor->GetChildActor());
	if (DoorBase1 && DoorBase1->IsOpened())
	{
		PathComp->FinishUsingCustomLink(LinkComp);
		return;
	}

	if(ActorInUse)
	{
		PathComp->FinishUsingCustomLink(LinkComp);
		return;
	}


	if (PathComp)
	{
		AActor* PathOwner = PathComp->GetOwner();
		AController* ControllerOwner = Cast<AController>(PathOwner);
		AAIController* AIController = Cast<AAIController>(ControllerOwner);
		if (ControllerOwner)
		{
			PathOwner = ControllerOwner->GetPawn();
		}
		ActorInUse = Cast<AFHCreatureBase>(PathOwner);
		NavLinkComp = LinkComp;

		if (ActorInUse && AIController)
		{
			PathComp->PauseMove();
			AIController->SetFocalPoint(GetActorLocation());
			float DoorOpenTime = ActorInUse->GetDoorOpenTime();
			if (DoorOpenTime)
			{
				GetWorldTimerManager().SetTimer(ResumeTimer, this, &ThisClass::ResumeActorMove, DoorOpenTime);
			}
			else {
				ResumeActorMove();
			}
		}
	}
}

void AFHDoor::ResumeActorMove()
{
	AAIController* AIController = UAIBlueprintHelperLibrary::GetAIController(ActorInUse);
	UPathFollowingComponent* PathFollowingComponent = AIController ? AIController->GetPathFollowingComponent() : nullptr;

	if (PathFollowingComponent && AIController)
	{

		AFHDoorBase* DoorBase1 = Cast<AFHDoorBase>(LeftDoorActor->GetChildActor());

		DoorBase1->OpenDoorForcefully(ActorInUse);


		AIController->ClearFocus(EAIFocusPriority::Gameplay);
		PathFollowingComponent->FinishUsingCustomLink(NavLinkComp);
		PathFollowingComponent->ResumeMove();
	}
	ActorInUse = nullptr;
	NavLinkComp = nullptr;
}
	
void AFHDoor::OnDoorCreated(AActor* Child)
{
	AFHDoorBase* DoorBase1 = Cast<AFHDoorBase>(LeftDoorActor->GetChildActor());
	AFHDoorBase* DoorBase2 = Cast<AFHDoorBase>(RightDoorActor->GetChildActor());

	if (DoorBase1 && DoorBase2)
	{
		DoorBase1->PairDoor = DoorBase2;
		DoorBase2->PairDoor = DoorBase1;
	}
}
