// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FHDungeonStructs.generated.h"

USTRUCT(BlueprintType)
struct FMinimapRoomSpec
{
	GENERATED_BODY()

	
	UPROPERTY()
	const class URoom* Room = nullptr;
	UPROPERTY()
	const class UFHRoomData* RoomData = nullptr;

	UPROPERTY()
	int32 MinFloor = 0;
	UPROPERTY()
	int32 MaxFloor = 0;
};