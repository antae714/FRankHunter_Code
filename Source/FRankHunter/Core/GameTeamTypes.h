// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GenericTeamAgentInterface.h"
#include "GameTeamTypes.generated.h"



UENUM()
enum class ETeamID : uint8
{
	Player,
	Creature

};



/**
 * 
 */
UCLASS()
class FRANKHUNTER_API UGameTeamTypes : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;


private:
	static ETeamAttitude::Type AttitudeSolver(FGenericTeamId FirstTeamID, FGenericTeamId SecondTeamID);
};
