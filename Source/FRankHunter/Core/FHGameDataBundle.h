// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Misc/Optional.h"
//#include "FHGameDataBundle.generated.h"

struct FFHGameDataBundle
{
	int32& PlayerMoney;
	int32& RequiredMoney;
	int32& LoopCount;
	int32& DeathCount;

	void Serialze(FArchive& Ar);
};


template<>
struct TStructOpsTypeTraits<FFHGameDataBundle> : public TStructOpsTypeTraitsBase2<FFHGameDataBundle>
{
	enum
	{
		WithSerializer = true
	};
};



