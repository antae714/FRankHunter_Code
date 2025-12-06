// Copyright F Rank Hunter. All Rights Reserved.


#include "Core/GameTeamTypes.h"



void UGameTeamTypes::Initialize(FSubsystemCollectionBase& Collection)
{
	//FGenericTeamId::SetAttitudeSolver(&UGameTeamTypes::AttitudeSolver);

}

ETeamAttitude::Type UGameTeamTypes::AttitudeSolver(FGenericTeamId FirstTeamID, FGenericTeamId SecondTeamID)
{
	return ETeamAttitude::Friendly;
}
