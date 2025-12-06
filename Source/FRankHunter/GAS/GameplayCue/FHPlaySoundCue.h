// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "FHPlaySoundCue.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHPlaySoundCue : public UGameplayCueNotify_Static
{
	GENERATED_BODY()
	

public:
	virtual bool HandlesEvent(EGameplayCueEvent::Type EventType) const override;

	virtual void HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters) override;
};
