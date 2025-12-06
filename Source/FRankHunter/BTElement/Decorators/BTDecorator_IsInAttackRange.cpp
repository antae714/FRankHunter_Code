// Copyright F Rank Hunter.. All Rights Reserved.


#include "BTElement/Decorators/BTDecorator_IsInAttackRange.h"
#include "AIController.h"
#include "Kismet/KismetSystemLibrary.h"

UBTDecorator_IsInAttackRange::UBTDecorator_IsInAttackRange()
{
	bNotifyTick = true; // 매틱마다 TickNode 호출
	INIT_DECORATOR_NODE_NOTIFY_FLAGS();
}

bool UBTDecorator_IsInAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AActor* TargetActor = TargetActorValue.GetValue<AActor>(OwnerComp);
	if (TargetActor)
	{
		AAIController* AC = OwnerComp.GetAIOwner();
		APawn* Pawn = AC ? AC->GetPawn() : nullptr;
		float AttackRangeValue = AttackRange.GetValue(OwnerComp);
		double DistanceSqrd = Pawn ? FVector::DistSquared2D(TargetActor->GetActorLocation(), Pawn->GetActorLocation()) : FLT_MAX;
		bool bIsInAttackRange = FMath::Square(AttackRangeValue) >= DistanceSqrd;

		return bIsInAttackRange;
	}
	else
	{
		return false;
	}
}

void UBTDecorator_IsInAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (CalculateRawConditionValue(OwnerComp, NodeMemory))
	{
		ConditionalFlowAbort(OwnerComp, EBTDecoratorAbortRequest::ConditionResultChanged);
	}
}
