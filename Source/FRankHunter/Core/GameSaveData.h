// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/FRankHunterTypes.h"
#include "GameSaveData.generated.h"

USTRUCT(BlueprintType)
struct FGameSaveData
{
	GENERATED_BODY()
public:
	int32 PlayerMoney{};
	int32 RequiredMoney{};
	int32 LoopCount{};
	int32 LoopCount_AtCurrentRank{};
	int32 DeathCount{};
	int32 CurrentReturnedCoreCount{};
	ELicenseRank TeamLicenseRank{};
	FText WhiteboardText;
	FString Password;
	FString GameName;
	uint32 bIsLAN : 1;
	uint32 bIsPublic : 1;
	uint32 bIsFriendOnly : 1;
	uint32 bIsMorning : 1;
	FString OwnerNickName;
	TArray<FName> SelectedPawnKey;

	bool Serialize(FArchive& Ar);

};

template<>
struct TStructOpsTypeTraits<FGameSaveData> : public TStructOpsTypeTraitsBase2<FGameSaveData>
{
	enum
	{
		WithSerializer = true,
	};
};


