// Copyright F Rank Hunter.. All Rights Reserved.


#include "BTElement/Decorators/BTDecorator_PercievedActor.h"

bool UBTDecorator_PercievedActor::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    return !!TargetActor.GetValue(OwnerComp);
}
