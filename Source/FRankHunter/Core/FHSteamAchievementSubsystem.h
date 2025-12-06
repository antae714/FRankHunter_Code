// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "FHSteamAchievementSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UFHSteamAchievementSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void UnlockAchievement(FName AchievementId);
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateAchievementProgress_Int(int32 Value, FName StatId);
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateAchievementProgress_Float(float Value, FName StatId);
};
