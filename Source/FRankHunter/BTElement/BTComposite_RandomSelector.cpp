// Copyright F Rank Hunter. All Rights Reserved.


#include "BTElement/BTComposite_RandomSelector.h"

UBTComposite_RandomSelector::UBTComposite_RandomSelector() 
{
	NodeName = "RandomSelector";
}

int32 UBTComposite_RandomSelector::GetNextChildHandler(FBehaviorTreeSearchData& SearchData, int32 PrevChild, EBTNodeResult::Type LastResult) const
{
	// success = quit
	int32 NextChildIdx = BTSpecialChild::ReturnToParent;

	if (PrevChild == BTSpecialChild::NotInitialized)
	{
		// newly activated: start from first
		NextChildIdx = 0;
		NextChildIdx = FMath::RandRange(0, Children.Num() - 1);
	}
	else if (LastResult == EBTNodeResult::Failed)
	{
		// failed = choose next child
		NextChildIdx = PrevChild + 1; 
		NextChildIdx = FMath::RandRange(0, Children.Num() - 1);
	}

	return NextChildIdx;
}

#if WITH_EDITOR

FName UBTComposite_RandomSelector::GetNodeIconName() const
{
	return FName("BTEditor.Graph.BTNode.Composite.Selector.Icon");
}

#endif

