// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "FHSampleGameplayCue.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API AFHSampleGameplayCue : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()
	

public:
	virtual void GameplayCueFinishedCallback() override;
};
