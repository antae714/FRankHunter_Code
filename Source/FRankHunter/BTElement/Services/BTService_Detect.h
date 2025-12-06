// Copyright F Rank Hunter.. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BehaviorTree/ValueOrBBKey.h"
#include "BTService_Detect.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UBTService_Detect : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_Detect();
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(Category = Node, EditAnywhere)
	FValueOrBBKey_Float DetectRange;

	UPROPERTY(Category = Node, EditAnywhere)
	FBlackboardKeySelector DetectedActor;

private:
	UPROPERTY()
	TArray<TWeakObjectPtr<class APlayerController>> PlayerControllers;
	void UpdatePlayerControllers();
	void DetectClosestPlayerController(UBehaviorTreeComponent& OwnerComp, float DeltaSeconds);
};
