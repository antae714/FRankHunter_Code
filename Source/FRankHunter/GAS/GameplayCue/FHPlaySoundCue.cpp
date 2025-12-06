// Copyright F Rank Hunter. All Rights Reserved.


#include "GAS/GameplayCue/FHPlaySoundCue.h"

bool UFHPlaySoundCue::HandlesEvent(EGameplayCueEvent::Type EventType) const
{
	return (EventType == EGameplayCueEvent::Executed);
}

void UFHPlaySoundCue::HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters)
{
	if (!IsValid(MyTarget))
	{
		return;
	}



}
