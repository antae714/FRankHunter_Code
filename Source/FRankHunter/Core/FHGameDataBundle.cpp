// Copyright F Rank Hunter. All Rights Reserved.


#include "Core/FHGameDataBundle.h"
#include "FRankHunter.h"

void FFHGameDataBundle::Serialze(FArchive& Ar)
{
	Ar.UsingCustomVersion(FFRankHunterCustomVersion::GUID);

	if (Ar.CustomVer(FFRankHunterCustomVersion::GUID) > FFRankHunterCustomVersion::FirstVersion)
	{
		Ar << PlayerMoney;
		Ar << RequiredMoney;
		Ar << LoopCount;
		Ar << DeathCount;
	}
}
