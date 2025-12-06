// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "AttributeSet.h"

#include "FHAttributeSet.generated.h"


UCLASS()
class FRANKHUNTER_API UFHAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	virtual void OnInitAbilityActorInfo();
};

