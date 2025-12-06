// Copyright F Rank Hunter. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EGateRank : uint8
{
	Invalid 		UMETA(DisplayName = "Invalid"),
	E 				UMETA(DisplayName = "E"),
	D 				UMETA(DisplayName = "D"),
	C 				UMETA(DisplayName = "C"),
	B 				UMETA(DisplayName = "B"),
	A 				UMETA(DisplayName = "A"),
	S 				UMETA(DisplayName = "S"),
};