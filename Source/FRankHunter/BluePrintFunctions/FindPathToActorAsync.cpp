// Copyright F Rank Hunter.. All Rights Reserved.


#include "FindPathToActorAsync.h"
#include "NavigationSystem.h"

UFindPathToActorAsync* UFindPathToActorAsync::FindPathToActorAsync(AController* Controller, AActor* TargetActor)
{
	UFindPathToActorAsync* Action = NewObject<UFindPathToActorAsync>();
	Action->RegisterWithGameInstance(Controller->GetWorld());
	if (Action->IsRegistered())
	{
		Action->TargetController = Controller;
		Action->TargetActor = TargetActor;
	}
	else
	{
		Action->SetReadyToDestroy();
	}
	return Action;
}

void UFindPathToActorAsync::Cancel()
{
	Super::Cancel();

	if (IsRegistered())
	{
		UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(TargetController->GetWorld());
		if (NavSystem)
		{
			NavSystem->AbortAsyncFindPathRequest(QueryID);
		}
	}
}

void UFindPathToActorAsync::Activate()
{
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(TargetController->GetWorld());
	if (NavSystem)
	{
		FNavAgentProperties properties = TargetController->GetNavAgentPropertiesRef();
		const ANavigationData* NavData = NavSystem->GetNavDataForProps(properties);
		if (!NavData)
		{
			SetReadyToDestroy();
			return;
		}

		const FVector StartLocation = TargetController->GetPawn()->GetActorLocation();
		const FVector EndLocation = TargetActor->GetActorLocation();
		FPathFindingQuery Query(TargetController, *NavData, StartLocation, EndLocation);

		FNavPathQueryDelegate delegate{};

		delegate.BindUObject(this, &UFindPathToActorAsync::OnPathFindCompletedDelegate);

		QueryID = NavSystem->FindPathAsync(properties, Query, delegate);
	}
	else
	{
		Cancel();
	}
}

void UFindPathToActorAsync::OnPathFindCompletedDelegate(uint32 UniqueQueryID, ENavigationQueryResult::Type QueryResult, FNavPathSharedPtr NavPath)
{
	FPathFindCompletParam param{};
	param.bIsSuccess = NavPath->IsPartial();
	
	OnPathFindCompleted.Broadcast(param);
	SetReadyToDestroy();
}
