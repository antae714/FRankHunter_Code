// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/ValueOrBBKey.h"
#include "BTTask_FindManaStone.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UBTTask_FindManaStone : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FindManaStone();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;


public:
	UPROPERTY(Category = Node, EditAnywhere)
	TSubclassOf<AActor> FindClass;

	UPROPERTY(Category = Node, EditAnywhere)
	FBlackboardKeySelector TargetActor;
};
