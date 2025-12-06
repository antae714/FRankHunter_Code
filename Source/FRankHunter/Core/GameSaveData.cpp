// Copyright F Rank Hunter. All Rights Reserved.


#include "Core/GameSaveData.h"
#include "FRankHunter.h"


bool FGameSaveData::Serialize(FArchive& Ar)
{
	Ar.UsingCustomVersion(FFRankHunterCustomVersion::GUID);
	if (Ar.CustomVer(FFRankHunterCustomVersion::GUID) >= FFRankHunterCustomVersion::FirstVersion)
	{
		Ar << PlayerMoney;
		Ar << RequiredMoney;
		Ar << LoopCount;
		Ar << LoopCount_AtCurrentRank;
		Ar << DeathCount;
		Ar << CurrentReturnedCoreCount;
		Ar << TeamLicenseRank;
		Ar << WhiteboardText;
		Ar << Password;
		Ar << GameName;
		
		bool IsLAN = bIsLAN;
		Ar << IsLAN;
		bIsLAN = IsLAN;
		bool IsPublic = bIsPublic;
		Ar << IsPublic;
		bIsPublic = IsPublic;
		bool IsFriendOnly = bIsFriendOnly;
		Ar << IsFriendOnly;
		bIsFriendOnly = IsFriendOnly;
		bool IsMorning = bIsMorning;
		Ar << IsMorning;
		bIsMorning = IsMorning;

		Ar << OwnerNickName;
		Ar << SelectedPawnKey;
	}

	return true;
}