// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "AttributeSet.h"
#include "BTService_AttributeSetBlackBoard.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UBTService_AttributeSetBlackBoard : public UBTService
{
	GENERATED_BODY()

public:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "Attribute")
	FGameplayAttribute TargetAttribute;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector BlackboardKey;
};
