// Copyright F Rank Hunter.. All Rights Reserved.


#include "BTElement/Tasks/BTTask_FindPatrolLocation.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

UBTTask_FindPatrolLocation::UBTTask_FindPatrolLocation()
{
	PatrolTargetKey.AddVectorFilter(this, NAME_None);
	PatrolRange = 0.0f;
}

EBTNodeResult::Type UBTTask_FindPatrolLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ensure(PatrolMaxRange.GetValue(OwnerComp) > 0.f);

	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControllingPawn)
	{
		return EBTNodeResult::Failed;
	}

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(ControllingPawn->GetWorld());
	if (!NavSystem)
	{
		return EBTNodeResult::Failed;
	}

	const FVector Origin = PatrolHomePosition.GetValue(OwnerComp);
	const float MinRange = PatrolMinRange.GetValue(OwnerComp);
	const float MaxRange = PatrolMaxRange.GetValue(OwnerComp);

	FNavLocation NextPatrolPos;
	bool bFound = false;

	const int32 MaxTries = 10;
	for (int32 i = 0; i < MaxTries; ++i)
	{
		if (NavSystem->GetRandomReachablePointInRadius(Origin, MaxRange, NextPatrolPos))
		{
			const float Dist = FVector::Dist(Origin, NextPatrolPos.Location);
			if (Dist >= MinRange)
			{
				bFound = true;
				break;
			}
		}
	}

	if (bFound)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(PatrolTargetKey.SelectedKeyName, NextPatrolPos.Location);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
