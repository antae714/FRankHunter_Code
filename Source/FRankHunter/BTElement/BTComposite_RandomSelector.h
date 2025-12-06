// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "BTComposite_RandomSelector.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UBTComposite_RandomSelector : public UBTCompositeNode
{
	GENERATED_BODY()
	UBTComposite_RandomSelector();

	virtual int32 GetNextChildHandler(struct FBehaviorTreeSearchData& SearchData, int32 PrevChild, EBTNodeResult::Type LastResult) const override;

#if WITH_EDITOR
	virtual FName GetNodeIconName() const override;
#endif
};
