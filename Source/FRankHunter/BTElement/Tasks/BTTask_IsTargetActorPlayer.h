// Copyright F Rank Hunter.. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/ValueOrBBKey.h"
#include "BTTask_IsTargetActorPlayer.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UBTTask_IsTargetActorPlayer : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UPROPERTY(Category = Node, EditAnywhere)
	FValueOrBBKey_Object TargetActor;

};
