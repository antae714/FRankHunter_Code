// Copyright F Rank Hunter.. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BehaviorTree/ValueOrBBKey.h"
#include "BTDecorator_PercievedActor.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UBTDecorator_PercievedActor : public UBTDecorator
{
	GENERATED_BODY()
public:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	UPROPERTY(Category = Node, EditAnywhere)
	FValueOrBBKey_Object TargetActor;
};
