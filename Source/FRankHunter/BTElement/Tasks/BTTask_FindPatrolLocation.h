// Copyright F Rank Hunter.. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/ValueOrBBKey.h"
#include "BTTask_FindPatrolLocation.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UBTTask_FindPatrolLocation : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FindPatrolLocation();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	UPROPERTY(Category = Node, EditAnywhere)
	FValueOrBBKey_Float PatrolMinRange;

	UPROPERTY(Category = Node, EditAnywhere)
	FValueOrBBKey_Float PatrolMaxRange;

	UPROPERTY(Category = Node, EditAnywhere)
	FValueOrBBKey_Float PatrolRange;

	UPROPERTY(Category = Node, EditAnywhere)
	FValueOrBBKey_Vector PatrolHomePosition;

	UPROPERTY(Category = Node, EditAnywhere)
	FBlackboardKeySelector PatrolTargetKey;
};
