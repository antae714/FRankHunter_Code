// Copyright F Rank Hunter.. All Rights Reserved.


#include "BTElement/Decorators/BTDecorator_TargetActorIsPlayer.h"

bool UBTDecorator_TargetActorIsPlayer::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	APawn* TargetActorValue = Cast<APawn>(TargetActor.GetValue(OwnerComp));
	if (TargetActorValue)
	{
		return !!Cast<APlayerController>(TargetActorValue->GetController());
	}

    return false;
}
